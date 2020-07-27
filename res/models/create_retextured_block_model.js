const fs = require('fs');


//console.log(process.argv.slice(2));

let args = null;
try {
	args = getParsedArgs();
} catch (err) {
	console.error(err);
}

const helpText = `
args:
	--name | -n
		The name of the model. Does not need to be preceeded by --name or -n if passed as the first argument.
		
	--parent | -p
		The name of the parent model. Defaults to "block/cube_all".
		
	--textures
		A comma-separated list of texture variable name/texture location pairs, with colons separating the texture variable name from the texture location.
		If --parent is "block/cube_all" (the default value), then --textures can default to "all: blocks/<--name>" (with the actual value of the --name argument in place of '<--name>').
`;


if (args != null) {
	if (args.help) {
		console.log(helpText + '\n');
	}
	
	let modelString = '{\n';
	modelString += `	parent: "${args.parent}",` + '\n';
	modelString += '	textures: {';
	if (args.textures.length === 1) {
		modelString += ` ${args.textures[0]} `;
	} else {
		modelString += '\n';
		args.textures.forEach(t => (modelString += (`		${t},` + '\n')));
		modelString += '	';
	}
	modelString += '},\n}';
	
	const fileName = `block/${args.name}.jonk`;
	fs.writeFileSync('./' + fileName, modelString);
	console.log(`Successfully created ${fileName}` + '\n');
}


function isValidModelName(name) {
	if (!name) return false;
	if (name.startsWith('-')) return false;
	return true;
}

function getParsedArgs() {
	const raw = process.argv.slice(2).map(a => a.trim());
	
	let parsed = {};
	
	let currentArg = null;
	for (let i = 0; i < raw.length; i++) {
		if (!currentArg) {
			const argName = raw[i];
			if (argName === '--help') {
				parsed.help = true;
				continue;
			}
			if ((argName === "--name") || (argName === '-n')) {
				currentArg = "name";
				if (i === (raw.length - 1)) throw "Argument 'name' requires a value.";
				continue;
			} else if ((argName === "--parent") || (argName === '-p')) {
				currentArg = "parent";
				if (i === (raw.length - 1)) throw "Argument 'parent' requires a value.";
				continue;
			} else if ((argName === "--textures") || (argName === '-t')) {
				currentArg = "textures";
				if (i === (raw.length - 1)) throw "Argument 'textures' requires a value.";
				continue;
			} else if (i === 0) {
				currentArg = "name";
			} else {
				throw `Unrecognized argument ${argName}. Use the --help argument to see a list of valid arguments.`;
			}
		}
		
		const argVal = raw[i];
		if (!isValidArgValue(argVal)) throw `Invalid value for argument '--${currentArg}'.`;
		if (parsed[currentArg]) throw `Duplicate argument '--${currentArg}'.`;
		if (currentArg === 'textures') {
			const pairs = argVal.split(',').map(p => p.trim()).filter(p => (!!p)).map((p, pi) => {
				const pairVals = p.split(':').map(k => k.trim());
				if (!pairVals[0]) throw `Texture ${pi + 1} is missing a texture variable name.`;
				if (!pairVals[1]) throw `Missing texture location for texture variable "${pairVals[0]}".`;
				return `${pairVals[0]}: "${pairVals[1]}"`;
			});
			if (pairs.length < 1) throw `No textures were specified.`;
			parsed[currentArg] = pairs;
		} else {
			parsed[currentArg] = argVal;
		}
		
		currentArg = null;
	}
	
	if (!parsed.name) throw `Missing required argument '--name'.`;
	if (!parsed.parent) parsed.parent = 'block/cube_all';
	if (!parsed.textures) {
		if (parsed.parent === 'block/cube_all') parsed.textures = [ `all: "blocks/${parsed.name}"` ];
		else throw "Missing required argument '--textures'.";
	}
	
	return parsed;
	
	function isValidArgValue(argVal) {
		return (!!argVal && !argVal.startsWith("-"));
	}
}
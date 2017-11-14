const Scaffold = require('scaffold-generator')
const mustache = require('mustache')
const argv = require('yargs')
  .usage('Usage: $0 --name moduleName [--id moduleId]')
  .version(false)
  .option('id', { describe: 'Unique module ID', type: 'number' })
  .option('name', { describe: 'Unique module name', type: 'string' })
  .demandOption(['name'], 'Please specify a module name')
  .argv;
let moduleName = '';
let moduleId = 0;

if (typeof argv.name === 'string') {
    moduleName = argv.name;
}

if (typeof argv.id === 'number' && !isNaN(argv.id)) {
    moduleId = argv.id;
}

if (!moduleName || moduleName === '') {
    console.error('Please set module name with --name');
    process.exit(1);
}

mustache.escape = v => v

new Scaffold({
  data: {
    Name: moduleName,
    moduleId,
  },
  render: mustache.render
})
.copy('templates/SynthModule', 'src/modules')
.then(() => {
  console.log(`${moduleName}Module generated with ID ${moduleId}. Please update ModuleFactory!`)
})

const binding = require('node-gyp-build')(__dirname)
const Libxml = binding.Libxml;
module.exports = Libxml;

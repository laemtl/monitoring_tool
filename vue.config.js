const path = require("path");
const webpack = require('webpack');

module.exports = {
  outputDir: path.resolve(__dirname, "./client/dist"),
  chainWebpack: config => {
    config
      .entry("app")
      .clear()
      .add("./client/src/main.js")
      .end();
    config.resolve.alias
      .set("@", path.join(__dirname, "./client/src"))
  },
  configureWebpack: config => {
    if(process.env.NODE_ENV === 'production') {
        config.optimization.minimizer[0].options.terserOptions.mangle = {
          reserved: ['$super'],
        }
     }
  }
}

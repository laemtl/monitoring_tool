const path = require("path");
const webpack = require("webpack");
module.exports = {
  devServer: {
    disableHostCheck: true
  },
  outputDir: path.resolve(__dirname, "./dist"),
  chainWebpack: config => {
    config
      .entry("app")
      .clear()
      .add("./src/main.js")
      .end();
    config.resolve.alias.set("@", path.join(__dirname, "./src"));
  },
  configureWebpack: config => {
    if (process.env.NODE_ENV === "production") {
      config.optimization.minimizer[0].options.terserOptions.mangle = {
        reserved: ["$super"]
      };
    }
  }
};

import "material-design-icons-iconfont/dist/material-design-icons.css"; // Ensure you are using css-loader
import Vue from "vue";
import "./plugins/vuetify";
import App from "./App.vue";

Vue.config.productionTip = false;

require("./assets/main.scss");

export const bus = new Vue();

new Vue({
  data: { 
    netInts: [],
    interval: 5,
    duration: 30
  },
  render: h => h(App)
}).$mount("#app");

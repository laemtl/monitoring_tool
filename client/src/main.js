import Vue from "vue";
import './plugins/vuetify'
import App from "./App.vue";

Vue.config.productionTip = false;

require("./assets/main.scss");

export const configBus = new Vue();

new Vue({
  render: h => h(App)
}).$mount("#app");
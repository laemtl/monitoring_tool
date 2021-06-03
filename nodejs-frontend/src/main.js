import "material-design-icons-iconfont/dist/material-design-icons.css"; // Ensure you are using css-loader
import Vue from "vue";
import Vuetify from 'vuetify';
import App from "./App.vue";
import 'vuetify/src/styles/main.sass'

Vue.config.productionTip = false;

require("./assets/main.scss");

export const bus = new Vue();

Vue.use(Vuetify, {
  iconfont: "md"
});

new Vue({
  vuetify : new Vuetify(),
  data: {
    netInts: [],
    interval: 5,
    duration: 30
  },
  render: h => h(App)
}).$mount("#app");

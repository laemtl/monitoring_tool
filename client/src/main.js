import Vue from "vue";
import './plugins/vuetify'
import App from "./App.vue";

Vue.config.productionTip = false;

require("./assets/main.scss");

export const configBus = new Vue();

new Vue({
  render: h => h(App)
}).$mount("#app");

var origin = new Date().getTime();

function accuTime(timer, max, repeatArgument, callbackArgument){
  var counter = 1;

  var init = (t) => {
    let timeStart = new Date().getTime();
    setTimeout(function () {
      if (counter < max) {
        let fix = (new Date().getTime() - timeStart) - timer;
        init(t - fix);
        counter++;
      
      // event to be repeated max times
        repeatArgument();
        
      } else {
      // event to be executed at animation end
        callbackArgument();
      }
    }, t);
  }
init(timer);
}
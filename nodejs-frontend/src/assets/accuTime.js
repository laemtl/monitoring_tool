exports.accuTime = (timer, max, repeatArgument, callbackArgument) => {
  var counter = 1;

  var init = t => {
    let timeStart = new Date().getTime();
    setTimeout(function() {
      if (counter < max) {
        let fix = new Date().getTime() - timeStart - timer;
        init(t - fix);
        counter++;

        // event to be repeated max times
        repeatArgument();
      } else {
        // event to be executed at animation end
        callbackArgument();
      }
    }, t);
  };
  init(timer);
};

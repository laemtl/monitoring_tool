<template>
  <v-flex xs12 sm6 md3 class="pa-0" v-show="graph.stat.active">
    <v-card flat>
      <v-toolbar dense flat color="brown lighten-4">
        <v-toolbar-title>{{ graph.stat.label }}</v-toolbar-title>
      </v-toolbar>
      <v-card-text>
        <p class="error" v-if="error">{{ error }}</p>
        <div class="panel-body" ref="panel">
          <div :class="'chart_container ref' + ucid" v-html="template"></div>
        </div>

        <div class="legend rickshaw_legend">
          <ul>
            <li v-for="(netInt, i) in this.graph.netInt" class="line">
              <a class="action">✔</a>
              <div
                class="swatch"
                :style="'background-color:' + colors[i] + ';'"
              ></div>
              <span class="label">{{ netInt }}</span>
            </li>
          </ul>
        </div>

        <div id="client" v-if="graph.id == 'client'"></div>
      </v-card-text>
    </v-card>
  </v-flex>
</template>

<script>
import Rickshaw from "rickshaw";
import "rickshaw/rickshaw.min.css";
import ucid from "ucid";
import { configBus } from "../main";

export default {
  name: "home",
  mixins: [ucid],
  props: ["graph"],
  data() {
    return {
      msgSeries: [],
      chart: [],
      colors: [],
      renderEveryNth: 1,

      //streamFrequency: 500,
      //messageIndex: 0,
      //displayedValues: [],

      error: null,
      template:
        "<div class='y_axis'></div><div class='chart'></div><div class='x_axis'></div><div class='legend'></div>"
      //legend: []
    };
  },
  beforeDestroy() {
    // Unregister the event listener before destroying this Vue instance
    window.removeEventListener("resize", this.onResize);
  },
  mounted() {
    this.initChart();

    configBus.$on(this.graph.id, message => {
      this.processMessage(message);
    });
  },
  watch: {
    graph: {
      handler: function(newValue, oldValue) {
        if (oldValue.netInt === newValue.netInt) return;
        this.clearChart();
      },
      deep: true
    }
  },
  methods: {
    getRandomColor() {
      var letters = "0123456789ABCDEF";
      var color = "#";
      for (var i = 0; i < 6; i++) {
        color += letters[Math.floor(Math.random() * 16)];
      }
      return color;
    },
    onResize(event) {
      this.resizeChart(this.chart[0]);
    },
    resizeChart(chart) {
      chart.configure({
        width: this.$refs.panel.clientWidth
      });
      chart.render();
    },
    getSeriesConfig() {
      var params = [];
      var len = this.graph.netInt.length;
      for (var i = 0; i < len; i++) {
        var color = this.getRandomColor();
        params.push({
          color: color,
          name: this.graph.netInt[i]
        });

        this.colors.splice(i, 0, color);
      }

      return new Rickshaw.Series.FixedDuration(params, undefined, {
        timeInterval: this.graph.interval * 1000,
        maxDataPoints: 100,
        timeBase: 2
      });
    },
    clearChart() {
      let p = this.chart[0].series.active; // preserve this 'needed' function
      this.chart[0].series = this.getSeriesConfig();
      this.chart[0].series.active = p; // unpreserve
      this.chart[0].update();
      //this.legend[0].render();
      this.resizeChart(this.chart[0]);
    },
    /* Rickshaw.js initialization */
    initChart() {
      var el = this;
      this.chart[0] = new Rickshaw.Graph({
        element: document.querySelector(
          ".chart_container.ref" + this.ucid + " .chart"
        ),
        width: "360",
        height: "180",
        renderer: this.graph.stat.type,
        min: 0,
        max: this.graph.stat.max,
        series: this.getSeriesConfig()
      });

      var y_axis = new Rickshaw.Graph.Axis.Y({
        graph: this.chart[0],
        orientation: "left",
        tickFormat: function(y) {
          return y.toFixed(4);
        },
        ticks: 5,
        element: document.querySelector(
          ".chart_container.ref" + this.ucid + " .y_axis"
        )
      });

      var x_axis = new Rickshaw.Graph.Axis.X({
        graph: this.chart[0],
        element: document.querySelector(
          ".chart_container.ref" + this.ucid + " .x_axis"
        ),
        orientation: "bottom",
        tickFormat: function(x) {
          if (x < 0) return "";
          //return el.parseTime(x);
          return x; //return x/100*60*2;
        }
      });

      /*this.legend[0] = new Rickshaw.Graph.Legend({
            	graph: this.chart[0],
	            element: document.querySelector(".chart_container.ref" + this.ucid + " .legend")
            });*/

      var shelving = new Rickshaw.Graph.Behavior.Series.Toggle({
        graph: this.chart[0]
        //legend: el.legend[0]
      });

      var highlighter = new Rickshaw.Graph.Behavior.Series.Highlight({
        graph: this.chart[0]
        //legend: el.legend[0]
      });

      var hoverDetail = new Rickshaw.Graph.HoverDetail({
        graph: this.chart[0],
        xFormatter: function(x) {
          return x;
          //return x/100*60*2
        },
        yFormatter: function(y) {
          return y;
        }
      });

      this.resizeChart(this.chart[0]);

      window.addEventListener("resize", this.onResize);
    },
    /*function compare(a, b) {
            if (a.freq < b.freq)
                return -1;
            if (a.freq > b.freq)
                return 1;
            return 0;
        },*/
    /* Insert received datapoints into the chart */
    insertDatapoints(messages, chart) {
      for (let i = 0; i < messages.length; i++) {
        //console.log(messages);

        var params = {};
        //for (var j = 0; j < this.graph.netInt.length; j++) {
        if (typeof messages[i].min !== "undefined") {
          params.Average = messages[i].avg;
        }

        if (typeof messages[i].min !== "undefined") {
          params.Min = messages[i].min;
        }

        if (typeof messages[i].max !== "undefined") {
          params.Max = messages[i].max;
        }

        if (typeof messages[i].rev_avg !== "undefined") {
          params.RevAvg = messages[i].rev_avg;
        }

        if (typeof messages[i].rev_min !== "undefined") {
          params.RevMin = messages[i].rev_min;
        }

        if (typeof messages[i].rev_max !== "undefined") {
          params.RevMax = messages[i].rev_max;
        }

        if (typeof messages[i].cfreq !== "undefined") {
          for (var j = 0; j < messages[i].cfreq.length; j++) {
            params[messages[i].cfreq[j].name] = messages[i].cfreq[j].freq;
          }

          if (this.graph.id == "client") {
            let clients = messages[i].cfreq;
            clients.sort(function(a, b) {
              return -(a.freq - b.freq);
            });

            let str = "";
            str += "<table>";

            for (let i = 0; i < clients.length; i++) {
              str += "<tr>";

              str += "<td>";
              str += clients[i].name;
              str += "</td>";

              str += "<td>";
              str += clients[i].freq;
              str += "</td>";

              str += "</tr>";
            }

            str += "</table>";
            document.getElementById("client").innerHTML = str;
          }
        }
        //}
        chart.series.addData(params);
      }
      chart.render();
    },
    parseTime(sec) {
      var hours = Math.floor(sec / 3600);
      var minutes = Math.floor((sec - hours * 3600) / 60);
      var seconds = sec - hours * 3600 - minutes * 60;

      var str = "";
      if (hours > 0) str += hours + "h";
      if (minutes > 0) str += minutes + "m";
      str += seconds + "s";
      return str;
    },
    /* Update displayed values every second on average */
    updateDisplayedValues() {
      if (this.messageIndex == this.streamFrequency) {
        this.messageIndex = 0;
        this.displayedValues = this.msgSeries;
      } else if (this.messageIndex == 0) {
        this.displayedValues = this.msgSeries;
        this.messageIndex++;
      } else {
        this.messageIndex++;
      }
    },
    processMessage(message) {
      //console.log(message.data);
      //if(this.graph.id == "reqRate") console.log(message);
      /* Check if displayed values have to be updated */
      //this.updateDisplayedValues();
      /* Push stream data to current series, if it's not yet render-time */
      if (this.msgSeries.length < this.renderEveryNth) {
        this.msgSeries.push(message.data);
        if(this.graph.id == 'tp') console.log(message.data);
      }

      /*if(typeof message.data.min !== 'undefined') {
                var i = this.graph.netInt.indexOf(message.netInt);  
                this.min.splice(i, 0, message.data.min.toFixed(4));
                this.max.splice(i, 0, message.data.max.toFixed(4));
            }*/

      /* Render-time! */
      if (this.msgSeries.length == this.renderEveryNth) {
        this.insertDatapoints(this.msgSeries, this.chart[0]);
        this.msgSeries = [];
      }
    }
  }
};
</script>

<style>
.panel-body {
  margin-left: 40px;
}

.chart_container {
  margin-top: 20px;
  position: relative;
}

.chart,
.x_axis {
  position: relative;
  //overflow: hidden;
}

div.y_axis {
  position: absolute;
  top: 0;
  bottom: 0;
  width: 40px;
  margin-left: -40px;
}

.y_axis {
  overflow: visible !important;
}

.legend .min,
.legend .max {
  padding-left: 10px;
}

#client {
  height: 50px;
  width: 200px;
  float: right;
  overflow-y: scroll;
}

#client td {
  padding-right: 20px;
}
</style>
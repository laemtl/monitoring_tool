<template>
  <v-flex xs12 sm6 md3 class="pa-0" v-show="metric.active">
    <v-card flat>
      <v-toolbar dense flat color="brown lighten-4">
        <v-toolbar-title>{{ metric.label }}</v-toolbar-title>
      </v-toolbar>
      <v-card-text>
        <p class="error" v-if="error">{{ error }}</p>
        <div class="panel-body" ref="panel">
          <div :class="'chart_container ref' + ucid" v-html="template"></div>
        </div>
        <div id="client" v-if="metric.id == 'client'"></div>
      </v-card-text>
    </v-card>
  </v-flex>
</template>

<script>
import Rickshaw from "rickshaw";
import "rickshaw/rickshaw.min.css";
import ucid from "ucid";
import { bus } from "../main";

export default {
  name: "home",
  mixins: [ucid],
  props: ["data"],
  data() {
    return {
      netInt: this.$root.netInts[this.data.netIntIndex].id,
      protocol: this.$root.netInts[this.data.netIntIndex].protocols[this.data.protocolIndex].id,
      metric: this.$root.netInts[this.data.netIntIndex].protocols[this.data.protocolIndex].metrics[this.data.metricId],
      msgSeries: [],
      chart: [],
      renderEveryNth: 1,
      error: null,
      template:
        "<div class='y_axis'></div><div class='chart'></div><div class='x_axis'></div>"
    };
  },
  beforeDestroy() {
    // Unregister the event listener before destroying this Vue instance
    window.removeEventListener("resize", this.onResize);
  },
  mounted() {
    this.initChart();

    bus.$on(this.metric.id, message => {
      this.processMessage(message);
    });
  },
  /*watch: {
    data: {
      handler: function(newValue, oldValue) {
        if (oldValue.netIntId === newValue.netIntId) return;
        this.clearChart();
      },
      deep: true
    }
  },*/
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
      let params = [{
        color: this.getRandomColor(),
        name: this.netInt + '-' + this.protocol
      }];

      return new Rickshaw.Series.FixedDuration(params, undefined, {
        timeInterval: this.$root.interval * 1000,
        maxDataPoints: 100,
        timeBase: 2
      });
    },
    clearChart() {
      let p = this.chart[0].series.active; // preserve this 'needed' function
      this.chart[0].series = this.getSeriesConfig();
      this.chart[0].series.active = p; // unpreserve
      this.chart[0].update();
      this.resizeChart(this.chart[0]);
    },
    // Rickshaw.js initialization
    initChart() {
      var el = this;
      this.chart[0] = new Rickshaw.Graph({
        element: document.querySelector(
          ".chart_container.ref" + this.ucid + " .chart"
        ),
        width: "360",
        height: "180",
        renderer: this.metric.type,
        min: 0,
        max: this.metric.max,
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
          return x;
        }
      });

      var shelving = new Rickshaw.Graph.Behavior.Series.Toggle({
        graph: this.chart[0]
      });

      var highlighter = new Rickshaw.Graph.Behavior.Series.Highlight({
        graph: this.chart[0]
      });

      var hoverDetail = new Rickshaw.Graph.HoverDetail({
        graph: this.chart[0],
        xFormatter: function(x) {
          return x;
        },
        yFormatter: function(y) {
          return y;
        }
      });

      this.resizeChart(this.chart[0]);

      window.addEventListener("resize", this.onResize);
    },
    
    // Insert received datapoints into the chart
    insertDatapoints(messages, chart) {
      for (let i = 0; i < messages.length; i++) {
        var params = {};
        if (typeof messages[i].avg !== "undefined") {
          params.Average = messages[i].avg;
        }

        if (typeof messages[i].min !== "undefined") {
          params.Min = messages[i].min;
        }

        if (typeof messages[i].max !== "undefined") {
          params.Max = messages[i].max;
        }

        if (typeof messages[i].cfreq !== "undefined") {
          for (var j = 0; j < messages[i].cfreq.length; j++) {
            params[messages[i].cfreq[j].name] = messages[i].cfreq[j].freq;
          }

          if (this.metric.id == "client") {
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
    // Update displayed values every second on average
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
      // Push stream data to current series, if it's not yet render-time
      if (this.msgSeries.length < this.renderEveryNth) {
        this.msgSeries.push(message.data);
      }

      // Render-time!
      if (this.msgSeries.length == this.renderEveryNth) {
        this.insertDatapoints(this.msgSeries, this.chart[0]);
        this.msgSeries = [];
      }
    }
  }
};
</script>
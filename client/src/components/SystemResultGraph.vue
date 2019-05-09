<template>
<v-flex xs12 sm6 md6>
      <v-card>
        <v-toolbar dense dark color="blue-grey darken-1">
            <v-toolbar-title>{{ name }}</v-toolbar-title>
            <v-spacer></v-spacer>
            {{displayedValues[0]}}
          </v-toolbar>
          <v-card-text>
            <p class="error" v-if="error">{{ error }}</p>
            <div class="panel-body">
              <!-- Chart container -->
              <div class="chart_container">
                <div :id="'y_axis_'+ucid" class="y_axis"></div>
                <div :id="'demo_chart_'+ucid" class="demo_chart" ref="panel"></div>
                <div :id="'x_axis_'+ucid" class="x_axis"></div>
              </div>
              <!-- End of chart container -->
            </div>
          </v-card-text>
      </v-card>
  </v-flex>
</template>

<script>
import Rickshaw from 'rickshaw'
import 'rickshaw/rickshaw.min.css'
import ucid from 'ucid'
import { configBus } from '../main'

export default {
    name: 'home',
    mixins: [
        ucid
    ],
    props: ['name'],
    data() {
        return {
            messageSeries: [],
            renderEveryNth: 3,
            updateInterval: 100,
            streamFrequency: 10,
            messageIndex: 0,
            displayedValues: [],
            error: null,
            chart: []
        }
    },
    mounted() {
        this.initChart();
        this.openSocketListeners();
    },
    watch: {
        renderEveryNth: function() {
            this.messageSeries = [];
            this.messageIndex = 0;
        }
    },
    methods: {
        /* Rickshaw.js initialization */
        initChart() {
            var el = this;
                
            this.chart[0] = new Rickshaw.Graph({
                element: document.querySelector("#demo_chart_" + this.ucid),
                width: "500",
                height: "180",
                renderer: "line",
                min: 0,
                max: 20,
                series: new Rickshaw.Series.FixedDuration([{
                    name: this.name,
                    color: '#EC644B'
                }], 
                undefined, 
                {
                    timeInterval: this.updateInterval,
                    maxDataPoints: 50,
                    timeBase: 0
                })
            });

            var y_axis = new Rickshaw.Graph.Axis.Y({
                graph: this.chart[0],
                orientation: 'left',
                tickFormat: function(y) {
                    return y.toFixed(1);
                },
                ticks: 5,
                element: document.getElementById('y_axis_' + this.ucid)
            });
            
            var x_axis = new Rickshaw.Graph.Axis.X({
                graph: this.chart[0],
                element: document.getElementById('x_axis_' + this.ucid),
                orientation: 'bottom',
                tickFormat: function(x){
                    if(x < 0)  return '';
                    return el.parseTime(x);
                }
            });

            this.resizeChart(this.chart[0]);

            window.addEventListener('resize', () => {
                el.resizeChart(el.chart[0])
            });
        },
        resizeChart(chart) {
            var el = this;
            console.log(el.$refs.panel);
            console.log(el.$refs.panel.clientWidth);
            
            chart.configure({
                width: el.$refs.panel.clientWidth,
            });
            chart.render();
        },
        /* Insert received datapoints into the chart */
        insertDatapoints(messages, chart) {
            for (let i = 0; i < messages.length; i++) {
                chart.series.addData({
                    value: parseInt(messages[i])
                })
            }
            chart.render();
        },
        parseTime(sec) {
            var hours   = Math.floor(sec / 3600);
            var minutes = Math.floor((sec - (hours * 3600)) / 60);
            var seconds = sec - (hours * 3600) - (minutes * 60);

            var str = "";
            if(hours > 0) str += hours+'h';
            if(minutes > 0) str += minutes+'m';
            str += seconds+'s';
            return str;
        },
        /* Update displayed values every second on average */
        updateDisplayedValues() {
            if (this.messageIndex == this.streamFrequency) {
                this.messageIndex = 0;
                this.displayedValues = this.messageSeries;
            } else if (this.messageIndex == 0) {
                this.displayedValues = this.messageSeries;
                this.messageIndex++;
            } else {
                this.messageIndex++;
            }
        },
        openSocketListeners() {
            this.ws = new WebSocket('ws://localhost:40510');
            var el = this;

            // event emmited when connected
            this.ws.onopen = function () {
                console.log('websocket is connected ...')

                // sending a send event to websocket server
                el.ws.send('connected')
            }

            // event emmited when receiving message 
            this.ws.onmessage = function (message) {
                var message = message.data;
        
                /* Check if displayed values have to be updated */
                el.updateDisplayedValues();

                /* Push stream data to current series, if it's not yet render-time */
                if (el.messageSeries.length < el.renderEveryNth) {
                    el.messageSeries.push(message);
                }

                /* Render-time! */
                if (el.messageSeries.length == el.renderEveryNth) {
                    el.insertDatapoints(el.messageSeries, el.chart[0]);
                    el.messageSeries = [];
                }
            }
        }
    }
}
</script>

<style>
.chart_container {
    margin-top: 20px;
    position: relative;
}

.demo_chart, .x_axis {
    position: relative;
    margin-left: 40px;
    overflow:hidden;
}

.y_axis {
    position: absolute;
    top: 0;
    bottom: 0;
    width: 40px;
    overflow: visible !important;
}
</style>
<template>
<v-flex xs12 sm6 md6>
      <v-card>
          <v-toolbar dense dark color="blue-grey darken-1">
              <v-toolbar-title>{{ name }}</v-toolbar-title>
          </v-toolbar>
          <v-card-text>
            <p class="error" v-if="error">{{ error }}</p>
            <div class="panel-body">
              <!-- Chart container -->
              <div class="chart_container">
                <div :id="'y_axis_'+ucid" class="y_axis"></div>
                <div :id="'demo_chart_'+ucid" class="demo_chart" ref="panel"></div>
              </div>
              <!-- End of chart container -->
            </div>
            
            <div class="panel-footer">
              <p v-if="displayedValues.length > 0">
                <small>
                  <span v-bind:style="{ color: dvColors.v1}">{{displayedValues[0]}} </span>
                </small>
              </p>
            </div>
          </v-card-text>
      </v-card>
  </v-flex>
</template>

<script>
    import Rickshaw from 'rickshaw'
    import 'rickshaw/rickshaw.min.css'
    import ucid from 'ucid'

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
                updateInterval: 20,
                streamFrequency: 10,
                messageIndex: 0,
                displayedValues: [],
                dvColors: {
                    v1: "#cb503a"
                },
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
                this.chart[0] = new Rickshaw.Graph({
                    element: document.querySelector("#demo_chart_" + this.ucid),
                    width: "500",
                    height: "180",
                    renderer: "line",
                    min: 0,
                    max: 20,
                    series: new Rickshaw.Series.FixedDuration([{
                        name: 'v1',
                        color: '#EC644B'
                    }], undefined, {
                        timeInterval: this.updateInterval,
                        maxDataPoints: 100,
                        timeBase: new Date().getTime() / 1000
                    })
                });

                var y_axis = new Rickshaw.Graph.Axis.Y({
                    graph: this.chart[0],
                    orientation: 'left',
                    tickFormat: function(y) {
                        return y.toFixed(1);
                    },
                    ticks: 5,
                    element: document.getElementById('y_axis_' + this.ucid),
                });

                this.resizeChart(this.chart[0]);

                window.addEventListener('resize', () => {
                    this.resizeChart(this.chart[0])
                });

            },
            resizeChart(chart) {
                chart.configure({
                    width: this.$refs.panel.clientWidth,
                });
                chart.render();
            },
            /* Insert received datapoints into the chart */
            insertDatapoints(messages, chart) {
                for (let i = 0; i < messages.length; i++) {
                    let voltageData = {
                        Magnitude1: messages[i]
                    };
                    chart.series.addData(voltageData);
                }
                chart.render();
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

<style scoped>
    .chart_container {
        padding-right: 40px;
        padding-bottom: 20px;
        margin-top: 20px;
        position: relative;
    }
    
    .demo_chart {
        position: relative;
        left: 40px;
    }
    
    .y_axis {
        position: absolute;
        top: 0;
        bottom: 0;
        width: 40px;
    }
    
    .footy {
        position: relative;
        width: 100%;
        margin-top: 50px;
        height: 60px;
        opacity: 0.2;
    }
    
    .glyphicon {
        color: #8E44AD;
        font-weight: bold;
    }
</style>
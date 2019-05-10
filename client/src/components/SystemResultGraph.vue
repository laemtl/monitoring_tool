<template>
<v-flex xs12 sm6 md3 class="pa-0">
    <v-card flat>
        <v-toolbar dense flat color="brown lighten-4">
            <v-toolbar-title>{{ name }}</v-toolbar-title>
          </v-toolbar>
          <v-card-text>
            <p class="error" v-if="error">{{ error }}</p>
            <div class="panel-body">
              <!-- Chart container -->
              <div class="chart_container">
                <div :id="'y_axis_'+ucid" class="y_axis"></div>
                <div :id="'demo_chart_'+ucid" class="demo_chart" ref="panel"></div>
                <div :id="'x_axis_'+ucid" class="x_axis"></div>
                <div :id="'legend'+ucid"></div>
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
            renderEveryNth: 1,
            updateInterval: 100,
            streamFrequency: 10,
            messageIndex: 0,
            displayedValues: [],
            colors: [],
            error: null,
            chart: []
        }
    },
    beforeDestroy() {
        // Unregister the event listener before destroying this Vue instance
        window.removeEventListener('resize', this.onResize)
    },
    mounted() {
        this.initChart();

        configBus.$on('message', message => {
            this.processMessage(message);
        });
    },
    watch: {
        renderEveryNth: function() {
            this.messageSeries = [];
            this.messageIndex = 0;
        }
    },
    methods: {
        getRandomColor() {
            var letters = '0123456789ABCDEF';
            var color = '#';
            for (var i = 0; i < 6; i++) {
                color += letters[Math.floor(Math.random() * 16)];
            }
            return color;
        },
        onResize(event) {
            this.resizeChart(this.chart[0]) 
        },
        resizeChart(chart) {
            chart.configure({
                width: this.$refs.panel.clientWidth,
            });
            chart.render();
        },
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
                series: new Rickshaw.Series.FixedDuration([
                    {
                        color: this.getRandomColor(),
                        name: 'netInt1'
                    }, 
                    {
                        color: this.getRandomColor(),
                        name: 'netInt2'
                    }, 
                    {
                        color: this.getRandomColor(),
                        name: 'netInt3'
                    }
                ], 
                undefined, 
                {
                    timeInterval: this.updateInterval,
                    maxDataPoints: 50,
                    timeBase: 0.1
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

            var legend = new Rickshaw.Graph.Legend({
            	graph: this.chart[0],
	            element: document.getElementById('legend' + this.ucid)
            });

            var shelving = new Rickshaw.Graph.Behavior.Series.Toggle({
                graph: this.chart[0],
                legend: legend
            });

            var highlighter = new Rickshaw.Graph.Behavior.Series.Highlight({
                graph: this.chart[0],
                legend: legend
            });

            var hoverDetail = new Rickshaw.Graph.HoverDetail({
                graph: this.chart[0],
                xFormatter: function(x) { return x.toFixed(2) },
                yFormatter: function(y) { return y }
            });

            this.resizeChart(this.chart[0]);

            window.addEventListener('resize', this.onResize);
        },
        /* Insert received datapoints into the chart */
        insertDatapoints(messages, chart) {
            for (let i = 0; i < messages.length; i++) {
                chart.series.addData({
                    netInt1: parseInt(messages[i]),
                    netInt2: parseInt(messages[i])*Math.random(),
                    netInt3: parseInt(messages[i])*Math.random()
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
        processMessage(message) {
            /* Check if displayed values have to be updated */
            this.updateDisplayedValues();

            /* Push stream data to current series, if it's not yet render-time */
            if (this.messageSeries.length < this.renderEveryNth) {
                this.messageSeries.push(message);
            }

            /* Render-time! */
            if (this.messageSeries.length == this.renderEveryNth) {
                this.insertDatapoints(this.messageSeries, this.chart[0]);
                this.messageSeries = [];
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
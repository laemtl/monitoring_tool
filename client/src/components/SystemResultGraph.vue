<template>
<v-flex xs12 sm6 md3 class="pa-0">
    <v-card flat>
        <v-toolbar dense flat color="brown lighten-4">
            <v-toolbar-title>{{ graph.name }}</v-toolbar-title>
        </v-toolbar>
        <v-card-text>
            <p class="error" v-if="error">{{ error }}</p>
            <div class="panel-body" ref="panel">
              <div :class="'chart_container ref'+ucid" v-html="template"></div>
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
    props: ['graph'],
    data() {
        return {
            messageSeries: [],
            renderEveryNth: 1,
            //streamFrequency: 500,
            messageIndex: 0,
            displayedValues: [],
            colors: [],
            error: null,
            chart: [],
            template: "<div class='y_axis'></div><div class='chart'></div><div class='x_axis'></div><div class='legend'></div>",
            legend: []
        }
    },
    beforeDestroy() {
        // Unregister the event listener before destroying this Vue instance
        window.removeEventListener('resize', this.onResize)
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
                //console.log(oldValue);
                //console.log(newValue);
                if(oldValue.netInt === newValue.netInt) return
                this.clearChart();
            },
            deep: true
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
        getSeriesConfig() {
            var params = [];
            var len = this.graph.netInt.length;
            for (var i = 0; i < len; i++) {
                params.push({
                    color: this.getRandomColor(),
                    name: this.graph.netInt[i]
                });
            }

            return new Rickshaw.Series.FixedDuration(
                params, 
                undefined, 
                {
                    timeInterval: this.graph.interval *  1000,
                    maxDataPoints: 100,
                    timeBase: 0
                }
            )
        },
        clearChart() {
            let p = this.chart[0].series.active; // preserve this 'needed' function
            this.chart[0].series = this.getSeriesConfig();
            this.chart[0].series.active = p; // unpreserve
            this.chart[0].update();
            this.legend[0].render();
            this.resizeChart(this.chart[0]);
        },
        /* Rickshaw.js initialization */
        initChart() {
            var el = this;
            this.chart[0] = new Rickshaw.Graph({
                element: document.querySelector(".chart_container.ref" + this.ucid + " .chart"),
                width: "360",
                height: "180",
                renderer: "line",
                min: 0,
                max: 0.015,
                series: this.getSeriesConfig()
            });

            var y_axis = new Rickshaw.Graph.Axis.Y({
                graph: this.chart[0],
                orientation: 'left',
                tickFormat: function(y) {
                    return y.toFixed(4);
                },
                ticks: 5,
                element: document.querySelector(".chart_container.ref" + this.ucid + " .y_axis")
            });
            
            var x_axis = new Rickshaw.Graph.Axis.X({
                graph: this.chart[0],
                element: document.querySelector(".chart_container.ref" + this.ucid + " .x_axis"),
                orientation: 'bottom',
                tickFormat: function(x){
                    if(x < 0)  return '';
                    //return el.parseTime(x);
                    return x //return x/100*60*2;
                }
            });

            this.legend[0] = new Rickshaw.Graph.Legend({
            	graph: this.chart[0],
	            element: document.querySelector(".chart_container.ref" + this.ucid + " .legend")
            });

            var shelving = new Rickshaw.Graph.Behavior.Series.Toggle({
                graph: this.chart[0],
                legend: el.legend[0]
            });

            var highlighter = new Rickshaw.Graph.Behavior.Series.Highlight({
                graph: this.chart[0],
                legend: el.legend[0]
            });

            var hoverDetail = new Rickshaw.Graph.HoverDetail({
                graph: this.chart[0],
                xFormatter: function(x) { return x 
                    //return x/100*60*2 
                },
                yFormatter: function(y) { return y }
            });

            this.resizeChart(this.chart[0]);

            window.addEventListener('resize', this.onResize);
        },
        /* Insert received datapoints into the chart */
        insertDatapoints(messages, chart) {
            for (let i = 0; i < messages.length; i++) {
                var params = {};
                for (var j = 0; j < this.graph.netInt.length; j++) {
                    params[this.graph.netInt[j]] = messages[i];
                }
                chart.series.addData(params);
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
            //this.updateDisplayedValues();
            
            /* Push stream data to current series, if it's not yet render-time */
            if (this.messageSeries.length < this.renderEveryNth) {
                this.messageSeries.push(message.data);
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
.panel-body{
    margin-left: 40px;
}

.chart_container {
    margin-top: 20px;
    position: relative;
}

.chart, .x_axis {
    position: relative;
    /**/
    overflow:hidden;
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
</style>

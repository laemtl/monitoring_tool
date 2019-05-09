import {Line, mixins} from 'vue-chartjs' // We specify what type of chart we want from vue-chartjs and the mixins module
import 'chartjs-plugin-streaming';
const { reactiveProp } = mixins

export default { 
  extends: Line,
  //We are extending the base chart class as mentioned above
  mixins: [reactiveProp],
  props: ["chartData"],
  data () {
    return {
      datacollection: {
        //Data to be represented on x-axis
        labels: ['January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'November', 'December'],
        datasets: [{
          label: 'Data One',
          backgroundColor: '#f87979',
          pointBackgroundColor: 'white',
          borderWidth: 1,
          pointBorderColor: '#249EBF',
          //Data to be represented on y-axis
          data: [40, 20, 30, 50, 90, 10, 20, 40, 50, 70, 90, 100]
        }]
      },
      //Chart.js options that controls the appearance of the chart
      options: { //Chart.js options
        scales: {
          yAxes: [{
            ticks: {
              beginAtZero: true
            },
            gridLines: {
              display: true
            }
          }],
          xAxes: [{
            type: 'realtime',
            gridLines: {
              display: false
            },
            realtime: {
              onRefresh: function(chart) {
                chart.data.datasets.forEach(function(dataset) {
                  dataset.data.push({
                    x: Date.now(),
                    y: Math.random()
                  });
                });
              },
              delay: 2000
            }
          }]
        },
        legend: {
          display: true
        },
        responsive: true,
        maintainAspectRatio: false
      }
    }
  },
  mounted () {
    //renderChart function renders the chart with the datacollection and options object.
    this.renderChart(this.datacollection, this.options)
  }  
}
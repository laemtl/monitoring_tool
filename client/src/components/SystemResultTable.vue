<template>
    <v-flex xs12 sm6 md4>
        <v-card>
            <v-toolbar dense dark color="blue-grey darken-1">
                <v-toolbar-title>Result</v-toolbar-title>
            </v-toolbar>
            <v-card-text>
                <p class="error" v-if="error">{{ error }}</p>

                <v-data-table
                    :headers="headers"
                    :items="statistics"
                    class="elevation-1"
                    hide-actions
                >
                    <template v-slot:items="props">
                    <td>{{ props.item }}</td>
                    <td>{{ statValue }}</td>
                    </template>
                </v-data-table>
            </v-card-text>
        </v-card>
    </v-flex>
</template>

<script>
  export default {
    name: 'SystemResultTable',
    props: ['statistics'],
    data() {
      return {
        headers: [{
            text: 'Name',
            align: 'left',
            sortable: false,
            value: 'name'
        },
        {
            text: 'Value',
            align: 'left',
            sortable: false,
            value: 'statValue'
        }],
        statValue: null,
        error: null
      }
    },
    methods: {
      connect() {
        this.ws = new WebSocket('ws://localhost:40510');
        var el = this;

        // event emmited when connected
        this.ws.onopen = function () {
          console.log('websocket is connected ...')

          // sending a send event to websocket server
          el.ws.send('connected')
        }

        // event emmited when receiving message 
        this.ws.onmessage = function (ev) {
          el.statValue = ev.data;
        }
      }
    },
    mounted : function () {
      console.log('Ready');
      this.connect();
    }
  }
</script>
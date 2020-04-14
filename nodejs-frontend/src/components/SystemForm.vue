<template>
  <v-container fluid class="pa-0">
    <v-navigation-drawer v-model="drawer" absolute temporary width="470">
      <v-flex xs12 pa-0>
        <v-toolbar dense dark color="primary">
          <v-toolbar-title>System analysis</v-toolbar-title>
        </v-toolbar>

        <v-form class="pa-3" ref="systemForm" v-on:submit="submit">
          <v-container class="px-0">
            <p class="error" v-if="error">{{ error }}</p>

            <v-card-text class="text-xs-center">
              <v-btn color="primary" class="my-0 ml-0" v-on:click="submit">Run</v-btn>
              <v-btn outline color="primary" class="my-0" v-on:click="stop">Stop</v-btn>
            </v-card-text>

            <v-layout class="pa-3">
              <v-flex md6 class="py-0">
                <v-text-field
                  class="pa-0 ma-0"
                  v-model="$root.interval"
                  :rules="[rules.requiredRules, rules.timeRules]"
                  validate-on-blur
                  label="Interval (s)"
                ></v-text-field>
              </v-flex>
              <v-flex md6 class="py-0">
                <v-text-field
                  class="pa-0 ma-0"
                  v-model="$root.duration"
                  :rules="[rules.requiredRules, rules.timeRules]"
                  validate-on-blur
                  label="Duration (s)"
                ></v-text-field>
              </v-flex>
            </v-layout>
            
            <SystemFormNetInts/>

          </v-container>
        </v-form>
      </v-flex>
    </v-navigation-drawer>
    <v-btn color="pink" dark @click.stop="drawer = !drawer">Open Form</v-btn>
  </v-container>
</template>

<script>
import SystemFormNetInts from "./SystemFormNetInts";
import { bus } from "../main";
import protocolsData from '../assets/json/protocols.json';

export default {
  name: "SystemForm",
  components: {
    SystemFormNetInts
  },
  data() {
    return {
      drawer: null,
      status: true,
      rules: {
        requiredRules: value => !!value || "Required",
        timeRules: value => (!isNaN(value) && value > 0) || "Invalid value"
      },
      error: null,
    };
  },
  methods: {
    submit() {
      if (this.$refs.systemForm.validate()) {
        bus.$emit("run");
        this.drawer = false;
      }
    },
    stop() {
      bus.$emit("stop");
    }
  },
  created: function() {
  },
  mounted: function() {
    console.log("mounted");
    bus.$on("formValidate", () => {
      this.$refs.systemForm.validate();
    }); 
  }
};
</script>
import {createApp} from 'vue'
import ElementPlus from 'element-plus'
import 'element-plus/dist/index.css'
import App from './App.vue'
import router from './router';
import {createPinia} from "pinia";


const app = createApp(App)
const pinia = createPinia()
// 使用 require 引入 vue-mathjax-next 并声明为 any 类型
// const VueMathjax = require('vue-mathjax-next') as any;

app.use(ElementPlus)
app.use(router)
app.use(pinia)

app.mount('#app')
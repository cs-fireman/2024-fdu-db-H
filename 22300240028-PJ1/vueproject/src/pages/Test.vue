<!-- <template>
  <h1>Test Page</h1>
  <el-button @click="jumpToLogin">登录界面</el-button>
</template> -->
<template>
  <div ref="login">
    <!-- 表单 -->
    <div class="login-content">
      <h1>{{ displayedText }}</h1>
      <el-button @click="jumpToLogin" class="login-button">login</el-button>
      <el-button @click="jumpToRegister" class="register-button">register</el-button>
    </div>

  </div>
</template>

<script setup>
import { ref, onMounted, onUnmounted } from "vue";
import * as THREE from "three";
import WAVES from "vanta/dist/vanta.waves.min";
import GLOBE from "vanta/dist/vanta.globe.min";
import { useRouter } from 'vue-router'
const router = useRouter()
function jumpToLogin() {
  router.push('/login')
}
function jumpToRegister() {
  router.push('/register')
}
const login = ref();
let vantaEffect;
const displayedText = ref(''); // 用于显示在界面上的文本
const fullText = 'Welcome To MyNotebook'; // 完整的文本内容
let textIndex = 0; // 当前字符的索引

function typeWriter() {
  if (textIndex < fullText.length) {
    displayedText.value += fullText.charAt(textIndex);
    textIndex++;
    setTimeout(typeWriter, 150); // 打字速度
  }
}



onMounted(() => {
  vantaEffect = GLOBE({
    el: login.value,
    THREE: THREE,
    mouseControls: true,
    touchControls: true,
    gyroControls: false,
    minHeight: 958.00,
    minWidth: 200.00,
    scale: 1.00,
    scaleMobile: 1.00
  });
  typeWriter();
});

onUnmounted(() => {
  if (vantaEffect) {
    vantaEffect.destroy();
  }
});
</script>

<!-- <style scoped>
</style> -->

<!-- <script setup>
import { useRouter } from 'vue-router'
const router = useRouter()
function jumpToLogin() {
  router.push('/login')
}

</script> -->

<style scoped>
.login-content {
  position: relative;
  z-index: 1000;
  top: 50%;
  /* Adjust vertical centering of the container */
  transform: translateY(500%);
  left: 10%;
  /* Adjust horizontal positioning of the container */
  width: 80%;
  /* Ensure the container does not span the entire width */
}

h1 {
  font-size: 66px;
  font-family: 'Roboto', sans-serif;
  color: #fff;
  text-align: left;
  /* Align the text to the left */
  margin-bottom: 20px;
  /* Space between the text and the button */
}

.login-button {
  border: 2px solid white;
  background: transparent;
  font-family: 'Roboto', sans-serif;
  color: white;
  width: 250px;
  height: 50px;
  font-size: 2em;
  border-radius: 5px;
  opacity: .5;
  position: absolute;
  /* Absolute positioning within the container */
  left: 0%;
  /* Adjust to slightly left */
  transform: translateX(130%);
  /* Slight left shift */
  transition: .3s;
}

.register-button {
  border: 2px solid white;
  background: transparent;
  font-family: 'Roboto', sans-serif;
  color: white;
  width: 250px;
  height: 50px;
  font-size: 2em;
  border-radius: 5px;
  opacity: .5;
  position: absolute;
  /* Absolute positioning within the container */
  left: 0%;
  /* Adjust to slightly left */
  transform: translateX(10%);
  /* Slight left shift */
  transition: .3s;
}

.register-button:hover {
  border: 2px solid #104F55;
  background-color: rgba(365, 365, 365, 0.5);
  cursor: pointer;
  color: #104F55;
  opacity: .8;
  transition: .3s;
  box-shadow: 0 8px 16px 0 rgba(0, 0, 0, 0.2);
}

.login-button:hover {
  border: 2px solid #104F55;
  background-color: rgba(365, 365, 365, 0.5);
  cursor: pointer;
  color: #104F55;
  opacity: .8;
  transition: .3s;
  box-shadow: 0 8px 16px 0 rgba(0, 0, 0, 0.2);
}
</style>

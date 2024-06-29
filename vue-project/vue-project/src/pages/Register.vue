<script setup>
import RegisterForm from "@/components/RegisterForm.vue";
import { ref, onMounted, onUnmounted } from "vue";
import * as THREE from "three";
import WAVES from "vanta/dist/vanta.waves.min";
import GLOBE from "vanta/dist/vanta.globe.min";
const register = ref();
let vantaEffect;
onMounted(() => {
    vantaEffect = GLOBE({
        el: register.value,
        THREE: THREE,
        mouseControls: true,
        touchControls: true,
        gyroControls: false,
        minHeight: 958.00,
        minWidth: 200.00,
        scale: 1.00,
        scaleMobile: 1.00
    });
});

onUnmounted(() => {
    if (vantaEffect) {
        vantaEffect.destroy();
    }
});
</script>

<template>
    <div class="container" ref="register">
        <div class="background">
            <RegisterForm class="form-container"></RegisterForm>

        </div>
    </div>
</template>

<style scoped>
* {
    box-sizing: border-box;
}

.container {


    display: flex;
    justify-content: center;
    /* 水平居中 */
    align-items: center;
    /* 垂直居中 */
    height: 100vh;
    /* 确保容器铺满整个视口 */
}

.background {

    /* 设置背景颜色为白色，透明度为0.5 */
    background-color: rgba(103, 186, 239, 0.9);
    /* 其他样式，例如内边距、边框圆角等 */
    padding: 20px;
    border-radius: 8px;
    box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
    /* 添加光影效果 */
}

.form-container {
    width: 400px;
    /* 设置表单容器宽度 */
    /* 其他样式 */
}
</style>
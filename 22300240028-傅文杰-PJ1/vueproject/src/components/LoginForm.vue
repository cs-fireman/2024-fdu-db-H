<script setup lang="ts">

import { reactive, ref } from 'vue'
import type { FormInstance, FormRules } from 'element-plus'
import { useRouter } from 'vue-router'
import { useUserstore } from '@/store/user'

const userStore = useUserstore()
const router = useRouter()

const ruleFormRef = ref<FormInstance>()

const ruleForm = reactive({
    userName: '',
    password: ''
})


const checkUserName = (rule: any, value: any, callback: any) => {
    if (value === '') {
        return callback(new Error('请输入用户名'))
    } else {
        callback()
    }
}

const checkPassword = (rule: any, value: any, callback: any) => {
    if (value === '') {
        callback(new Error('请输入密码'))
    } else {
        callback()
    }
}

const rules = reactive<FormRules<typeof ruleForm>>({
    userName: [{ validator: checkUserName, trigger: 'blur' }],
    password: [{ validator: checkPassword, trigger: 'blur' }],
})
import { LoginApi } from "@/request/api";
import { ElMessage } from 'element-plus'

const submitForm = (formEl: FormInstance | undefined) => {
    if (!formEl) return
    formEl.validate(async (valid) => {
        if (valid) {
            console.log('表单验证成功，可以提交')
            let res = await LoginApi({
                username: ruleForm.userName,
                password: ruleForm.password
            })
            console.log(res);
            console.log('login', res.user.id)
            if (res.success) {
                ElMessage.success('登陆成功')
                userStore.userName = ruleForm.userName
                userStore.userId = parseInt(res.user.id)
                // console.log('userStore:', userStore.userId)
                // // 保存登录状态到 localStorage
                // localStorage.setItem("Flag", "isLogin");
                // await router.push({ name: 'IndexMain', params: { userName: ruleForm.userName } });
                await router.push({ path: '/index' })
            } else {
                ElMessage.error('登陆失败，请重新输入用户名和密码')
            }
        } else {
            ElMessage.error('登陆失败，未输入用户名和密码')
            return false
        }
    })
}

function jumpToRegister() {
    router.push('/register')
}
function jumpToLastPage() {
    router.push('/')
}

</script>

<template>
    <el-form ref="ruleFormRef" :model="ruleForm" :rules="rules" style="max-width: 600px" label-width="auto"
        class="demo-ruleForm">

        <el-form-item label="用户名" prop="userName">
            <el-input v-model="ruleForm.userName" type="text" autocomplete="off" />
        </el-form-item>

        <el-form-item label="密码" prop="password">
            <el-input v-model="ruleForm.password" type="password" autocomplete="off" />
        </el-form-item>

        <el-form-item>
            <el-button type="success" @click="submitForm(ruleFormRef)">登录
            </el-button>
            <el-button type="primary" @click="jumpToRegister()">注册
            </el-button>
            <el-button type="primary" @click="jumpToLastPage()">返回首页
            </el-button>
        </el-form-item>

    </el-form>
</template>

<style scoped></style>
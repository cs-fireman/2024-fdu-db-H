# vue3+typescript模板项目记录

## 1基础环境配置

### 1.1 项目初始化

针对本次课程涉及到的功能点进行环境配置

首先安装node.js（在cmd输入npm可以检查是否已经安装）

接下来在创建项目的文件夹下打开cmd，输入

```shell
npm create vue@latest
```

开始创建项目，除了typescript选择是以外，其他暂时不选择

接下来输入以下指令实现项目包安装以及启动

```shell
cd <project-name>
npm install 
npm run dev
```

在浏览器输入 http://localhost:5173/查看vue官方界面，在代码编辑器上可以选择vscode、webstrom、IntelliJ IDEA（浏览器可以安装vue开发组件在编码时调试）

### 1.2 安装 Vue 的 TypeScript 类型定义文件

```shell
npm install --save-dev @types/vue
```

在tsconfig.json文件夹下配置

```typescript
  "compilerOptions": {
    "typeRoots": ["./node_modules/@types", "./types"]
  }
```

在src下新建types文件夹,新建vue-shims.d.ts

```typescript
declare module '*.vue' {
    import Vue from 'vue';
    export default Vue;
}
```



### 1.3 前端组件库安装

这里选择element-plus组件库(vue2使用element-ui)，同时可以选择使用bootstrapVue组件库

element-plus官网：[一个 Vue 3 UI 框架 | Element Plus (element-plus.org)](https://element-plus.org/zh-CN/)

```shell
# 选择一个你喜欢的包管理器

# NPM
$ npm install element-plus --save

# Yarn
$ yarn add element-plus

# pnpm
$ pnpm install element-plus
```

完整引用:

在main.ts中引入

```typescript
// main.ts
import { createApp } from 'vue'
import ElementPlus from 'element-plus'
import 'element-plus/dist/index.css'
import App from './App.vue'

const app = createApp(App)

app.use(ElementPlus)
app.mount('#app')
```

### 1.4 request api封装

```shell
npm install axios
```

在安装了包之后，在src下新建request文件，接着新建http.ts和api.ts

```typescript
//http.ts
import axios from 'axios'
// 创建axios实例
const request = axios.create({
    baseURL: '',// 所有的请求地址前缀部分
    timeout: 80000, // 请求超时时间(毫秒)
    withCredentials: true,// 异步请求携带cookie
    headers: {
        'Content-Type': 'application/json',
    }
})
request.defaults.withCredentials = true;

export default request
```

```typescript
//api.ts
import instance from "@/request/http";

//一般情况下，接口类型会放到一个文件
// 下面三个TS接口，表示要传的参数
interface ReqLogin {
    username: string
    password: string
}

interface ReqRegister {
    username: string
    password: string
    first_name: string,
    last_name: string,
    email: string
}

interface ReqStatus {
    id: string
    navStatus: string
}


// Res是返回的参数，T是泛型，需要自己定义，返回对数统一管理
type Res<T> = Promise<ItypeAPI<T>>;
// 标注了一些一般情况下响应数据返回的参数
interface ItypeAPI<T> {
    success: string | null // 返回状态码的信息，如请求成功等;
    result: T,//请求的数据，用泛型
    msg: string | null // 返回状态码的信息，如请求成功等
    message:string
    code: number //返回后端自定义的200，404，500这种状态码
    user: User
    users: User[]
    total_users: number
    total_pages: number
}

interface User {
    is_active: boolean,
    username: string,
    email: string,
    is_superuser: boolean,
    first_name: string,
    last_name: string
}



//以下是封装的api模板:

//  get请求，没参数，
export const FlashSessionListApi = (): Res<null> =>
    instance.get("/flashSession/list");

// get请求，有参数，(如果你不会写类型也可以使用any,不过不建议,因为用了之后 和没写TS一样)
export const AdminListAPI = (params: any): any =>
    instance.get("/admin/list", {params});

// get请求，有参数，路径也要传参  (也可能直接在这写类型，不过不建议,大点的项目会维护一麻烦)
export const ProductCategoryApi = (params: { parentId: number }): any =>
    instance.get(`/productCategory/list/${params.parentId}`, {params});

// post请求 ，没参数
export const LogoutAPI = (): Res<null> =>
    instance.post("/admin/logout");

// post请求，有参数,如传用户名和密码
export const loginAPI = (data: ReqLogin): Res<string> =>
    instance.post("/admin/login", data);

// post请求 ，没参数，但要路径传参
export const StatusAPI = (data: ReqStatus): Res<null> =>
    instance.post(`/productCategory?ids=${data.id}&navStatus=${data.navStatus}`);

```

调用方式 ：

```typescript
import { TestHelloApi} from "@/request/api";
async function TestClick() {
  let res = await TestHelloApi()
}
```



### 1.5 路由管理

```shell
npm install vue-router@nex
```



```typescript
//main.ts 添加
import router from './router';
app.use(router)
```

在src下新建router文件夹，在该文件夹下新建index.ts文件进行路由配置

这里展示了一般的路由配置

（在跳转路由时可以通过router-link或者router-view结合router.push/router.replace实现，在指定跳转的组件时，可以使用name或者path，一般来说组件的name就是默认的文件名，当然也可以在.vue文件中新建<script>然后重新命名，这样的好处是在路由跳转设置时更加灵活）

```typescript
import {createRouter, createWebHistory} from "vue-router";

import Login from '@/pages/Login.vue';
import Index from "@/pages/Index.vue";
import Test from "@/pages/Test.vue";
import IndexMain from "@/components/IndexMain.vue";
import CheckUserInfo from "@/components/CheckUserInfo.vue";
import AddUser from "@/components/AddUser.vue";
import Register from "@/pages/Register.vue";

const routes =
    [
        {
            path: '/',
            name: 'Login',
            component: Login
        },
        {
            path: '/register',
            name: 'Register',
            component: Register
        },
        {
            path: '/index',
            name: 'Index',
            component: Index,
            children: [
                {
                    path: '',
                    name:'IndexMain',
                    component: IndexMain,
                },
                {
                    path: 'checkUserInfo',
                    component: CheckUserInfo,
                },
                {
                    path: 'addUser',
                    component: AddUser,
                },
            ]

        },
        {
            path: '/test',
            name: 'Test',
            component: Test
        }
    ];

const router = createRouter({
    history: createWebHistory(),
    routes
});

export default router;

```



### 1.6 状态管理

```shell
npm i pinia
```



```typescript
//main.ts 添加
import {createPinia} from "pinia";
const pinia=createPinia()
app.use(pinia)
```

在src下新建store文件夹，然后创建user.ts

```typescript
//user.ts
import {defineStore} from "pinia";

export const useUserstore = defineStore(
    'user',
    {
        state() {
            return {
                userName:'userName'
            }
        }
    }
)
```



### 1.7 代理

```typescript
//vite.config.ts
server: {
    // /** 设置 host: true 才可以使用 Network 的形式，以 IP 访问项目 */
    // host: true, // host: "0.0.0.0"
    // /** 端口号 */
    // port: 3333,
    // /** 是否自动打开浏览器 */
    // open: false,
    /** 跨域设置允许 */
    cors: true,
    /** 端口被占用时，是否直接退出 */
    strictPort: false,
    /** 接口代理 */
    proxy: {
      "/api/": {
        // target: "https://mock.mengxuegu.com/mock/63218b5fb4c53348ed2bc212",
        target: "http://127.0.0.1:8080",
        ws: true,
        /** 是否允许跨域 */
        changeOrigin: true,  // 是否改变域
        rewrite: (path) => path.replace(/^\/api/, ''),
      }
    },
  }

```



### 1.8 最终项目的main.ts

```typescript
import {createApp} from 'vue'
import ElementPlus from 'element-plus'
import 'element-plus/dist/index.css'
import App from './App.vue'
import router from './router';
import {createPinia} from "pinia";

const app = createApp(App)
const pinia = createPinia()

app.use(ElementPlus)
app.use(router)
app.use(pinia)

app.mount('#app')
```



## 2 功能实现

### 2.1 登录功能

#### 登录page

在pages文件夹下创建Login.vue然后在components文件夹下创建LoginForm.vue组件

```vue
//Login.vue
<script setup lang="ts">

import LoginForm from "@/components/LoginForm.vue";
</script>

<template>
  <div class="container">
    <div class="background">
      <LoginForm class="form-container"></LoginForm>

    </div>
  </div>
</template>

<style scoped>
* {
  box-sizing: border-box;
}

.container {
  background-image: url('@/assets/images/background.jpeg');
  background-size: cover; /* 让背景图片覆盖整个屏幕 */
  background-position: center; /* 让背景图片居中显示 */
  display: flex;
  justify-content: center; /* 水平居中 */
  align-items: center; /* 垂直居中 */
  height: 100vh; /* 确保容器铺满整个视口 */
}
.background {

  /* 设置背景颜色为白色，透明度为0.5 */
  background-color: rgba(103, 186, 239, 0.9);
  /* 其他样式，例如内边距、边框圆角等 */
  padding: 20px;
  border-radius: 8px;
  box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); /* 添加光影效果 */
}
.form-container {
  width: 400px; /* 设置表单容器宽度 */
  /* 其他样式 */
}
</style>

```



#### 登录表单组件

```vue
<script setup lang="ts">

import {reactive, ref} from 'vue'
import type {FormInstance, FormRules} from 'element-plus'
import {useRouter} from 'vue-router'
import {useUserstore} from '@/store/user'

const userStore=useUserstore()
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
  userName: [{validator: checkUserName, trigger: 'blur'}],
  password: [{validator: checkPassword, trigger: 'blur'}],
})
import {LoginApi} from "@/request/api";
import {ElMessage} from 'element-plus'

const submitForm = (formEl: FormInstance | undefined) => {
  if (!formEl) return
  formEl.validate(async (valid) => {
    if (valid) {
    console.log('表单验证通过，可以提交')
    }
  })
}



</script>

<template>
  <el-form
      ref="ruleFormRef"
      :model="ruleForm"
      :rules="rules"
      style="max-width: 600px"
      label-width="auto"
      class="demo-ruleForm"
  >

    <el-form-item label="用户名" prop="userName">
      <el-input v-model="ruleForm.userName" type="text" autocomplete="off"/>
    </el-form-item>

    <el-form-item label="密码" prop="password">
      <el-input v-model="ruleForm.password" type="password" autocomplete="off"/>
    </el-form-item>

    <el-form-item>
      <el-button type="primary" @click="submitForm(ruleFormRef)"
      >登录
      </el-button
      >
      <el-button type="success"
      >注册
      </el-button
      >
    </el-form-item>

  </el-form>
</template>

<style scoped>

</style>
```

#### 登录api封装

```typescript
//登录 api
export const LoginApi = (data: ReqLogin): Res<null> =>
    instance.post('/api/login', data);
```

#### 登录api使用

```typescript
import {LoginApi} from "@/request/api";


if (valid) {
      let res = await LoginApi({
        username: ruleForm.userName,
        password: ruleForm.password
      })
      // console.log(res)
      if (res.success) {
        ElMessage.success('登陆成功')
        userStore.userName=ruleForm.userName
        await router.push({ name: 'IndexMain', params: { userName: ruleForm.userName } });

      } else {
        ElMessage.error('登陆失败，请重新输入用户名和密码')
      }
    } else {
      ElMessage.error('登陆失败，未输入用户名和密码')
      return false
    }
```

#### 添加跳转至注册界面按钮功能

```typescript
function jumpToRegister() {
  router.push('/register')
}
```



### 2.2 注册功能

#### 注册page

```vue
//Register.vue
<script setup lang="ts">
import RegisterForm from "@/components/RegisterForm.vue";
</script>

<template>
  <div class="container">
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
  background-image: url('@/assets/images/background.jpeg');
  background-size: cover; /* 让背景图片覆盖整个屏幕 */
  background-position: center; /* 让背景图片居中显示 */
  display: flex;
  justify-content: center; /* 水平居中 */
  align-items: center; /* 垂直居中 */
  height: 100vh; /* 确保容器铺满整个视口 */
}
.background {

  /* 设置背景颜色为白色，透明度为0.5 */
  background-color: rgba(103, 186, 239, 0.9);
  /* 其他样式，例如内边距、边框圆角等 */
  padding: 20px;
  border-radius: 8px;
  box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); /* 添加光影效果 */
}
.form-container {
  width: 400px; /* 设置表单容器宽度 */
  /* 其他样式 */
}
</style>
```



#### 注册表单组件

```vue
//RegisterForm.vue
<script setup lang="ts">
import {reactive, ref} from 'vue'
import type {FormInstance, FormRules} from 'element-plus'
import {useRouter} from 'vue-router'
import {ElMessage} from 'element-plus'
import {RegisterApi} from "@/request/api";
const router = useRouter()

const ruleFormRef = ref<FormInstance>()

const registerForm = reactive({
  userName: '',
  password: '',
  first_name:'',
  last_name:'',
  email:'',
})

const submitForm = (formEl: FormInstance | undefined) => {
  if (!formEl) return
  formEl.validate(async (valid) => {
 if (valid) {
     console.log('注册表单提交')
 }
  })
}


</script>

<template>
  <el-form
      ref="ruleFormRef"
      :model="registerForm"
      style="max-width: 600px"
      label-width="auto"
      class="demo-ruleForm"
  >

    <el-form-item label="用户名" prop="userName">
      <el-input v-model="registerForm.userName" type="text" autocomplete="off"/>
    </el-form-item>

    <el-form-item label="密码" prop="password">
      <el-input v-model="registerForm.password" type="password" autocomplete="off"/>
    </el-form-item>

    <el-form-item label="名" prop="first_name">
      <el-input v-model="registerForm.first_name" type="text" autocomplete="off"/>
    </el-form-item>

    <el-form-item label="姓" prop="last_name">
      <el-input v-model="registerForm.last_name" type="text" autocomplete="off"/>
    </el-form-item>

    <el-form-item label="邮箱" prop="email">
      <el-input v-model="registerForm.email" type="text" autocomplete="off"/>
    </el-form-item>

    <el-form-item>
      <el-button type="success" @click="submitForm(ruleFormRef)"
      >注册
      </el-button
      >
      <el-button type="primary"
      >登录
      </el-button
      >
    </el-form-item>

  </el-form>
</template>

<style scoped>


</style>
```



#### 注册api封装

```typescript
//注册 api
export const RegisterApi = (data: ReqRegister): Res<null> =>
    instance.post('/api/register', data);
```

#### 注册api使用

```typescript
if (valid) {
      let res = await RegisterApi({
        username: registerForm.userName,
        password: registerForm.password,
        first_name: registerForm.first_name,
        last_name: registerForm.last_name,
        email: registerForm.email
      })
      if (res.success) {
        ElMessage.success('注册成功')
        await router.push('/');
      } else {
        ElMessage.error('注册失败请重新输入')
      }
    } else {
      ElMessage.error('注册失败请重新输入')
      return false
    }
```



#### 补充跳转至登陆界面功能

```typescript
function jumpToLogin() {
  router.push('/')
}
```



### 2.3主页布局

```vue
//Index.vue
<script setup lang="ts">
import AsideNavBar from "@/components/AsideNavBar.vue";
import Header from "@/components/Header.vue";
import IndexMain from "@/components/IndexMain.vue";

</script>

<template>
  <div class="common-layout">
    <el-container>
      <el-header>
        <Header>

        </Header>
      </el-header>
      <el-container>
        <el-aside width="200px">
          <AsideNavBar></AsideNavBar>
        </el-aside>

        <el-container>
          <el-main>

            <router-view></router-view>

          </el-main>
        </el-container>

      </el-container>

    </el-container>
  </div>
</template>
	
<style scoped>

</style>
```



### 2.4 Header和登出功能

#### Header.vue组件

```vue
<script setup lang="ts">
    
</script>

<template>
  <div style="display: flex; justify-content: space-between; align-items: center;">
    <img src="https://img2.imgtp.com/2024/04/05/DMHKG7pg.jpg" alt="Logo" style="height: 50px;">
    <div>
      <el-button type="info" @click="logout">登出</el-button>
    </div>
  </div>
</template>

<style scoped>

</style>
```

#### 登出api封装

```typescript
//登出 api
export const LogoutApi = (): Res<null> =>
    instance.get('/api/logout');
```

#### 登出api使用

```typescript
//Header.vu
import {ElMessage, ElNotification as notify} from 'element-plus'
import {LogoutApi} from "@/request/api";
import {useRouter} from 'vue-router'
const router = useRouter()
async function logout() {
  let res = await LogoutApi()
  if (res.success) {
    ElMessage.success("登出成功")
    await router.push('/')
  } else {
    ElMessage("登出失败")
  }
  console.log(res)
}
```



### 2.5 侧边栏组件

```vue
//AsideNavBar.vue
<script setup lang="ts">
import {
  Document,
  Menu as IconMenu,
  Setting,
} from '@element-plus/icons-vue'
import {useRouter} from 'vue-router'
const router = useRouter()

function change(key: string, keyPath: string[]) {
  console.log(key)
}
</script>

<template>
  <el-menu
      default-active="1"
      class="el-menu-vertical-demo"
      :router=true
  >
    <el-menu-item index="/index/">
      <el-icon>
        <icon-menu/>
      </el-icon>
      <span>个人信息</span>
    </el-menu-item>
    <el-menu-item index="/index/checkUserInfo">
      <el-icon>
        <document/>
      </el-icon>
      <span>查看用户信息</span>
    </el-menu-item>
    <el-menu-item index="/index/addUser">
      <el-icon>
        <setting/>
      </el-icon>
      <span>添加用户</span>
    </el-menu-item>
  </el-menu>
</template>

<style scoped>
</style>
```



### 2.6 显示当前用户信息

#### 用户显示界面组件

```vue
//Profile.vue
<script setup lang="ts">
import {GetUserInfoByUserName} from "@/request/api";
import {ElMessage} from "element-plus";
import {onBeforeMount, ref} from "vue";
import {useUserstore} from '@/store/user'

const userStore=useUserstore()

let user = ref({
  username: '',
  first_name: '',
  last_name: '',
  email: '',
  avatar: 'https://img2.imgtp.com/2024/04/05/QUYeUsws.jpg', // 假设的头像URL
})

async function getUserInfo() {
  let res = await GetUserInfoByUserName({
    userName: userStore.userName
  })
  // console.log(res)
  if (res.success) {
    user.value.username = res.user.username
    if (res.user.email === "")
      user.value.email = res.user.first_name + res.user.last_name + "@example.com"
    else
      user.value.email = res.user.email
    user.value.first_name = res.user.first_name
    user.value.last_name = res.user.last_name
  } else {
    ElMessage.error('个人信息查询失败')
  }
}

onBeforeMount(() => {
  getUserInfo()
});


</script>

<template>
  <div class="user-profile">
    <img :src="user.avatar" alt="User Avatar" class="avatar"/>
    <h2>{{ user.username }}</h2>
    <p><strong>Full Name:</strong> {{ user.first_name }} {{ user.last_name }}</p>
    <p><strong>Email:</strong> {{ user.email }}</p>
  </div>
</template>

<style scoped>
.user-profile {
  max-width: 300px;
  margin: 20px auto;
  padding: 20px;
  border: 1px solid #ccc;
  border-radius: 10px;
  text-align: center;
}

.user-profile .avatar {
  width: 100px;
  height: 100px;
  border-radius: 50%;
  margin-bottom: 20px;
}
</style>
```



#### 用户查询api

```typescript
//根据username查询用户信息api  get
export const GetUserInfoByUserName = (params: { userName: string }): Res<null> =>
    instance.get(`/api/find/${params.userName}`, {params});
```



### 2.7查看用户列表

#### 用户列表界面组件

```vue
<script setup lang="ts">
import {onBeforeMount, ref} from "vue";
import {GetUserInfoByPageNum} from "@/request/api";

interface User {
  userName: string
  first_name: string
  last_name: string
  email: string
}

const tableData = ref<User[]>([]);

let currentPage = ref(1);
let pageSize = ref(10);
let total = ref(100);

onBeforeMount(async () => {
  let res = await GetUserInfoByPageNum({
    pageNumber: 1
  })
  res.users.forEach(item => {
    tableData.value.push({
      userName: item.username,
      first_name: item.first_name,
      last_name: item.last_name,
      email: item.email
    });
  });
  total.value = res.total_users
  console.log('total_pages.value:',total.value)
})


const handleCurrentChange = (newPage: number) => {
  currentPage.value = newPage;
  fetchData();
};
const fetchData = async () => {
  // 在这里调用 API 获取数据，使用 currentPage 作为参数
  let res = await GetUserInfoByPageNum({
    pageNumber: currentPage.value
  })
  tableData.value=[]
  res.users.forEach(item => {
    tableData.value.push({
      userName: item.username,
      first_name: item.first_name,
      last_name: item.last_name,
      email: item.email
    });
  });
  total.value = res.total_users
};


</script>

<template>
  <el-row>
    <el-col :span="24">
      <el-table :data="tableData" stripe style="width: 100%">
        <el-table-column prop="userName" label="用户名" width="180"/>
        <el-table-column prop="first_name" label="姓" width="180"/>
        <el-table-column prop="last_name" label="名" width="180"/>
        <el-table-column prop="email" label="邮箱"/>
      </el-table>
    </el-col>
  </el-row>

  <el-pagination
      @current-change="handleCurrentChange"
      :current-page="currentPage"
      :page-size="pageSize"
      layout="prev, pager, next"
      :total="total"
  />

</template>

<style scoped>

</style>
```



#### 用户列表获取api

```typescript
//根据pageNumber查询用户信息api  get
export const GetUserInfoByPageNum = (params: { pageNumber: number }): Res<null> =>
    instance.get(`/api/users/list/${params.pageNumber}`, {params});
```
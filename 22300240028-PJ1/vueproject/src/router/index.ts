import {createRouter, createWebHistory} from "vue-router";

import Test from "@/pages/Test.vue";
import Login from "@/pages/Login.vue";
import Index from "@/pages/Index.vue";
import Register from "@/pages/Register.vue";
import CheckUserInfo from "@/pages/CheckUserInfo.vue";
import MyNotesList from "@/pages/MyNotesList.vue";
import MyNotes from "@/pages/MyNotes.vue";
import Calender from "@/pages/Calender.vue";
import TodoList from "@/pages/TodoList.vue";
const routes =
    [
        {
            path: '/',
            name: 'Test',
            component: Test
        },
        {
            path: '/login',
            name: 'Login',
            component: Login
        },
        {
            path: '/index',
            name: 'Index',
            component: Index,
        },
        {
            path: '/register',
            name: 'Register',
            component: Register
        },
        {
            path: '/index/checkUserInfo',
            name: 'CheckUserInfo',
            component: CheckUserInfo
        },
        {
            path: '/index/myNotesList',
            name: 'MyNotesList',
            component: MyNotesList
        },
        {
            path: '/index/myNotesList/myNotes/:note_id', // 添加动态路由
            name: 'MyNotes',
            component: MyNotes,
            props: true // 启用 props 将 route.params 映射到组件的 props 中
        },
        {
            path: '/index/calender',
            name: 'Calender',
            component: Calender
        },
        {
            path: '/index/todolist',
            name: 'TodoList',
            component: TodoList
        }
    ];

const router = createRouter({
    history: createWebHistory(),
    routes
});

export default router;

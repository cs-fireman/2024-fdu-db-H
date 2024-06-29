<template>
    <div class="common-layout">
        <el-container>
            <el-header>
                <Header></Header>
            </el-header>
            <el-container>
                <el-aside width="200px">
                    <AsideBarNotesList></AsideBarNotesList>
                </el-aside>
                <el-main>
                    <el-scrollbar height="800px">
                        <div v-for="(item, index) in tableData" :key="index">
                            <el-descriptions class="margin-top" :title="item.title" :column="1" :size="size" border>
                                <template #extra>
                                    <el-button type="primary" @click="gotoMynote(item.note_id)">编辑正文</el-button>
                                    <!-- <el-button type="primary">修改标题</el-button> -->
                                    <el-button type="primary" @click="deleteNote(item.note_id)">删除</el-button>
                                </template>
                                <el-descriptions-item>
                                    <template #label>
                                        <div class="cell-item" style="width: 80px;">
                                            <el-icon :style="iconStyle">
                                                <Clock />
                                            </el-icon>
                                            更新日期
                                        </div>
                                    </template>
                                    <div>
                                        {{ item.date }}
                                    </div>
                                </el-descriptions-item>
                                <el-descriptions-item>
                                    <template #label>
                                        <div class="cell-item">
                                            <el-icon :style="iconStyle">
                                                <tickets />
                                            </el-icon>
                                            摘要
                                        </div>
                                    </template>
                                    <div class="remark-content">
                                        {{ item.remark }}
                                    </div>

                                </el-descriptions-item>
                            </el-descriptions>
                        </div>
                    </el-scrollbar>
                </el-main>
            </el-container>
        </el-container>
    </div>
</template>
  
<script setup lang="ts">
import { GetUserNotes, DeleteNote } from "@/request/api";
import { ElMessage } from "element-plus";
import { useUserstore, useNotestore } from '@/store/user'
import Header from "@/components/Header.vue";
import AsideBarNotesList from "@/components/AsideBarNotesList.vue";
import { onBeforeMount, ref, watchEffect } from "vue";
import { computed } from 'vue'
import {
    Tickets,
    Clock,
    CirclePlus
} from '@element-plus/icons-vue'
import type { ComponentSize } from 'element-plus'
import router from "@/router";
// 组件
const size = ref<ComponentSize>('default')

const iconStyle = computed(() => {
    const marginMap = {
        default: '6px',
    }
    return {
        marginRight: marginMap[size.value] || marginMap.default,
    }
})
const blockMargin = computed(() => {
    const marginMap = {
        default: '28px',
    }
    return {
        marginTop: marginMap[size.value] || marginMap.default,
    }
})

// TODO
interface Note {
    title: string
    date: string
    remark: string
    note_id: number
}

const tableData = ref<Note[]>([]);

const userStore = useUserstore()
const noteStore = useNotestore()

// 监听 noteStore.notes 并更新 tableData
watchEffect(() => {
    tableData.value = noteStore.notes.map(note => ({
        title: note.note_title,
        date: note.update_time,
        remark: note.note_content,
        note_id: note.note_id
    }));
})

async function getUserNotes() {
    console.log('in MyNotesList', userStore.userName)
    let res = await GetUserNotes({
        userName: userStore.userName
    })
    console.log(res)
    if (res && res.notes) {
        noteStore.setNotes(res.notes);
    }
    return res;
}

const gotoMynote = (note_id: number) => {
    console.log('note_id:', note_id)
    router.push('/index/myNotesList/myNotes/' + note_id)
}

const deleteNote = (note_id: number) => {
    console.log('note_id:', note_id);
    deleteData(note_id);
}
const deleteData = async (noteId: number) => {
    let res = await DeleteNote({
        noteId: noteId
    });
    console.log('deleteData', res);
    if (res.success) {
        ElMessage.success('删除成功');
        noteStore.deleteNoteById(noteId);
    } else {
        ElMessage.error('删除失败');
    }

};

onBeforeMount(async () => {
    let res = await getUserNotes();
    // res.notes.forEach(item => {
    //     tableData.value.push({
    //         title: item.note_title,
    //         date: item.update_time,
    //         remark: item.note_content,
    //         note_id: item.note_id
    //     });
    // });
    // console.log('tableData:', tableData.value)
});

</script>
<style scoped>
.el-descriptions {
    margin-top: 20px;
    border: 1px solid #ebebeb;
    /* 添加边框，颜色可以按你的设计调整 */
    border-radius: 4px;
    /* 如果你想要边角带圆角可以添加这一行 */
    padding: 20px;

}

.cell-item {
    display: flex;
    align-items: center;
}

.margin-top {
    margin-top: 20px;
}

.remark-content {
    overflow: hidden;
    white-space: nowrap;
    text-overflow: ellipsis;
    width: 700px;
}
</style>

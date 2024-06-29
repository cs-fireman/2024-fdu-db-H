import {defineStore} from "pinia";
interface Note {
    note_id: number,
    user_id: number,
    note_content: string,
    note_title: string,
    update_time: string,
    // user_name: string
}
export const useUserstore = defineStore(
    'user',
    {
        state() {
            return {
                userName:'userName',
                userId: 0,
            }
        }
    },
)
export const useNotestore = defineStore(
    'notes',
    {
        state: () => ({
            // 初始化 notes 为 Note 类型的空数组
            notes: [] as Note[]
        }),
        actions: {
            // Action 来添加一个 note 到数组
            addNote(newNote: Note) {
                this.notes.push(newNote);
                this.sortNotesByUpdateTime();
            },
            // 设置整个 notes 数组
            setNotes(newNotes: Note[]) {
                this.notes = newNotes;
                this.sortNotesByUpdateTime();
            },
            getNoteContentById(noteId: number): string | null {
                const foundNote = this.notes.find(note => note.note_id === noteId);
                return foundNote ? foundNote.note_content : null;
            },
            // 更新 note 内容
            updateNoteContent(noteId: number, newContent: string, update_time: string) {
                const foundNote = this.notes.find(note => note.note_id === noteId);
                if (foundNote) {
                    foundNote.note_content = newContent;
                    foundNote.update_time = update_time;
                }
                console.log('update note content');
                this.sortNotesByUpdateTime(); // 更新内容后重新排序
            },
            // 获取最大的 note_id
            getMaxNoteId(): number {
                if (this.notes.length === 0) {
                    return 0;  // 如果没有笔记，返回 0 或者你可以定义其他的默认值
                }
                return Math.max(...this.notes.map(note => note.note_id));
            },
            // 删除笔记的方法
            deleteNoteById(noteId: number) {
                this.notes = this.notes.filter(note => note.note_id !== noteId);
            },
            sortNotesByUpdateTime() {
                this.notes.sort((a, b) => {
                    // 解析日期字符串为 Date 对象
                    console.log('in sort time')
                    const dateA = new Date(a.update_time.replace(/(\d{4})\/(\d{1,2})\/(\d{1,2}) (\d{1,2}):(\d{1,2}):(\d{1,2})/, function(match, year, month, day, hour, minute, second) {
                        return `${year}-${month.padStart(2, '0')}-${day.padStart(2, '0')}T${hour.padStart(2, '0')}:${minute.padStart(2, '0')}:${second.padStart(2, '0')}`;
                    }));
                    const dateB = new Date(b.update_time.replace(/(\d{4})\/(\d{1,2})\/(\d{1,2}) (\d{1,2}):(\d{1,2}):(\d{1,2})/, function(match, year, month, day, hour, minute, second) {
                        return `${year}-${month.padStart(2, '0')}-${day.padStart(2, '0')}T${hour.padStart(2, '0')}:${minute.padStart(2, '0')}:${second.padStart(2, '0')}`;
                    }));
                    // console.log(a.update_time, b.update_time);
                    // console.log(dateA, dateB, dateB.getTime() - dateA.getTime());
                    // demo test
                    // const testDateStr = '2024/5/9 16:37:18';
                    // const formattedDateStr = testDateStr.replace(/(\d{4})\/(\d{1,2})\/(\d{1,2}) (\d{1,2}):(\d{1,2}):(\d{1,2})/, function(match, year, month, day, hour, minute, second) {
                    //     return `${year}-${month.padStart(2, '0')}-${day.padStart(2, '0')}T${hour.padStart(2, '0')}:${minute.padStart(2, '0')}:${second.padStart(2, '0')}`;
                    // });
                    // console.log(formattedDateStr);
                    // console.log(new Date(formattedDateStr));
                    // 比较两个日期的时间戳
                    return dateB.getTime() - dateA.getTime();
                });
            },
        }
    },
)
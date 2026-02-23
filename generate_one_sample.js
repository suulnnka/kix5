import { spawn } from 'child_process';
import fs from 'fs';

const Egaroucid_DEPTH = 12
const Game_Per_File = 100
const Loop_Number = 100
const POOL_SIZE = 4

class ProcessPool {
    constructor(poolSize) {
        this.poolSize = poolSize;
        this.taskQueue = [];
        this.workers = [];
        this.activeWorkers = 0;
    }

    async runTask(taskFn) {
        return new Promise((resolve, reject) => {
            this.taskQueue.push({ taskFn, resolve, reject });
            this.processQueue();
        });
    }

    processQueue() {
        while (this.activeWorkers < this.poolSize && this.taskQueue.length > 0) {
            const { taskFn, resolve, reject } = this.taskQueue.shift();
            this.activeWorkers++;
            
            taskFn()
                .then(result => {
                    resolve(result);
                })
                .catch(error => {
                    reject(error);
                })
                .finally(() => {
                    this.activeWorkers--;
                    this.processQueue();
                });
        }
    }

    async waitAll() {
        while (this.taskQueue.length > 0 || this.activeWorkers > 0) {
            await new Promise(resolve => setTimeout(resolve, 100));
        }
    }
}

const WHITE = 2
const BLACK = 1
const DUMMY = 3
const SPACE = 0

function d2_to_d1(x,y){
    return (x)*9+y
}

const direction = [-10, -9, -8, -1, 1, 8, 9, 10]

function check_way(board, move, way, color){
	let count=0;
	for(let now=move+direction[way] ; board[now]!=DUMMY && board[now]!=SPACE ; now+=direction[way] ){
		if(board[now]==color){
            if(count>0){
                return true;
            }else{
                return false;
            }
        }
		count++;
	}
	return false;
}

function filp(board, move_string, color){
    let x = move_string[1].codePointAt() - "0".codePointAt()
    let y = move_string[0].codePointAt() - "a".codePointAt() + 1
    let move = d2_to_d1(x,y)

    let newBoard = [...board]

    for(let way = 0; way<8; way++){
		if(check_way(board, move,way, color) == false) continue;
		for(let now=move+direction[way] ; newBoard[now]!=color ; now+=direction[way] )
			newBoard[now]=color;
	}

    newBoard[move] = color
    return newBoard
}

function color_to_char(color){
    if(color == WHITE) return "O";
    if(color == BLACK) return "X";
    return "."
}

function draw_board(board){
    let char_board = ""
    for(let x = 1 ; x <= 8 ; x++){
        for(let y = 1 ; y <= 8 ; y++){
            let move = d2_to_d1(x,y)
            char_board += color_to_char(board[move])
        }
    }
    return char_board
}

function generate_sample(board, color, moves, scores,last_move){
    let char_board = draw_board(board)
    let sample = {
        board: char_board,
        color: color_to_char(color),
        moves: moves,
        scores: scores,
        last_move: last_move
    }
    
    return sample
}

function choose_move(moves, scores){
    let list = []
    for(let i = 0 ; i < moves.length ; i++){
        list.push({
            move: moves[i],
            score: parseInt(scores[i])
        })
    }
    list.sort((a, b) => {
        return b.score - a.score
    })

    let best_score = list[0].score
    list = list.filter((item) => item.score >= best_score - 6)

    let min_score = list[list.length - 1].score
    list.forEach((item) => {
        item.score =  item.score - min_score + 1
    })

    let score_sum = list.reduce((acc, cur) => acc + cur.score, 0)
    let random_number = Math.random() * score_sum
    let current_score = 0
    for(let i = 0 ; i < list.length ; i++){
        current_score += list[i].score
        if(current_score >= random_number){
            return list[i].move
        }
    }
}

function generate_value_sample_one_game() {
    let board = [
        3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0,
        3, 0, 0, 0, 0, 0, 0, 0, 0,
        3, 0, 0, 0, 0, 0, 0, 0, 0,
        3, 0, 0, 0, 2, 1, 0, 0, 0,
        3, 0, 0, 0, 1, 2, 0, 0, 0,
        3, 0, 0, 0, 0, 0, 0, 0, 0,
        3, 0, 0, 0, 0, 0, 0, 0, 0,
        3, 0, 0, 0, 0, 0, 0, 0, 0,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3]

    let samples = []
    let last_move = ""

    const child = spawn('./Egaroucid/Egaroucid_for_Console_7_8_0_SIMD.exe', ["-l", Egaroucid_DEPTH.toString()], {
        stdio: ['pipe', 'pipe', 'pipe']
    });

    let text = ""
    let status = "go"

    function wait(time) {
        return new Promise(resolve => setTimeout(resolve, time))
    }

    function parseHint(text) {
        let color
        if (text.includes("WHITE to move")) {
            color = WHITE
        }else{
            color = BLACK
        }
        const lines = text.split("\n")
        let moves = []
        let scores = []
        for (let i = 1; i < lines.length; i++) {
            let line = lines[i]
            if (line[0] != "|") break
            let elements = line.split("|")
            moves.push(elements[3].trim())
            scores.push(elements[4].trim())
        }

        samples.push(generate_sample(board, color, moves, scores,last_move))

        let move = choose_move(moves, scores)
        last_move = move
        board = filp(board, move, color)

        return move
    }

    child.stdout.on('data', (data) => {
        text += data.toString()
    });

    child.stderr.on('data', async (data) => {
        await wait(0)
        //process.stdout.write(text)
        if (text.includes("GAME OVER")) {
            child.stdin.write('exit\n');
            return;
        }
        if (status == "go") {
            status = "hint"
            child.stdin.write('hint 30\n');
        } else if (status == "hint") {
            status = "go"
            let play = parseHint(text)

            child.stdin.write('play ' + play + '\n');
        }
        text = ""
    });

    return new Promise((resolve, reject) => {
        child.on('close', (code) => {
            if (code === 0) {
                resolve({
                    last_board: draw_board(board),
                    moves:samples,
                })
            } else {
                reject(new Error(`Child process with exit code ${code}`))
            }
        })
    })

}

let top_p = 0
function get_time_string(){
    const now = new Date();

    const year = String(now.getFullYear()).slice(-2);  // 获取后两位年份
    const month = String(now.getMonth() + 1).padStart(2, '0');  // 月份从0开始
    const day = String(now.getDate()).padStart(2, '0');
    const hours = String(now.getHours()).padStart(2, '0');
    const minutes = String(now.getMinutes()).padStart(2, '0');
    const seconds = String(now.getSeconds()).padStart(2, '0');

    top_p ++;
    let str_top_p = top_p.toString().padStart(3, '0');
    const result = `${day}${month}${year}_${hours}${minutes}${seconds}.${str_top_p}`;
    return result
}

async function generate_samples(){
    let samples_list = []
    for(let i = 0 ; i < Game_Per_File ; i ++){
        let start_time = Date.now()
        let samples = await generate_value_sample_one_game()
        let end_time = Date.now()
        samples_list.push(samples)
        console.log(`Game ${i+1} completed, ${end_time - start_time} ms`);
    }
    let time = get_time_string()
    fs.writeFileSync(`one_network/samples_depth_${Egaroucid_DEPTH}_${time}.txt`, JSON.stringify(samples_list,null,2))
}

const main_pool = new ProcessPool(POOL_SIZE);
for(let x = 1 ; x <= Loop_Number ; x++){
    main_pool.runTask(() => generate_samples());
}
main_pool.waitAll();



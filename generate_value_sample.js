import { spawn } from 'child_process';
import fs from 'fs'

const Egaroucid_DEPTH = 12

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

function generate_sample(board, color, score){
    let sample = ""
    for(let x = 1 ; x <= 8 ; x++){
        for(let y = 1 ; y <= 8 ; y++){
            let move = d2_to_d1(x,y)
            sample += color_to_char(board[move])
        }
    }
    sample += " " + color_to_char(color) + " " + score
    return sample
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

        for(let i = 0 ; i < moves.length ; i++){
            let move = moves[i]
            let score = scores[i]
            let new_board = filp(board, move, color)
            let sample = generate_sample(new_board, 3 - color, score)
            samples.push(sample)
        }

        let random_number = Math.floor(Math.random() * moves.length)
        let move = moves[random_number]
        let score = scores[random_number]
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
                resolve(samples)
            } else {
                reject(new Error(`Child process with exit code ${code}`))
            }
        })
    })

}

async function generate_samples(){
    let top = fs.readFileSync('value_network/top.txt', 'utf8').trim()
    top = parseInt(top) + 1
    let file = fs.createWriteStream(`value_network/samples_depth_${Egaroucid_DEPTH}_${top}.txt`, {flags: 'a'});
    for(let i = 0 ; i < 10 ; i ++){
        let start_time = Date.now()
        let samples = await generate_value_sample_one_game()
        let end_time = Date.now()
        console.log(`Game ${i+1} completed, ${samples.length} samples written, ${end_time - start_time} ms`);
        for(let sample of samples){
            file.write(sample + "\n");
        }
    }
    file.end();
    fs.writeFileSync('value_network/top.txt', top.toString())
}

for(let x = 1 ; x <= 100 ; x++){
    await generate_samples()
}



const { execFileSync } = require('child_process');
const fs = require('fs');  

const Egaroucid_DEPTH = 10
const one_file_sample_number = 500
const first_random_move = 8

// 6 step 100 game 544 ms       1x
// 8 step 100 game 2283 ms      4x
// 10 step 100 game 13920 ms   20x
// 12 step 100 game 25139 ms   40x

function generate_policy_sample(){
    let top = 0
    try{
        top = fs.readFileSync('./data_policy/top.txt', 'utf8').trim()
        top = parseInt(top)
    }catch(err){}
    top = top + 1

    const stdout = execFileSync('./Egaroucid/Egaroucid_for_Console_7_8_0_SIMD.exe', 
        [
            '-l', 
            Egaroucid_DEPTH.toString(),
            '-sf',
            one_file_sample_number.toString(),
            first_random_move.toString(),
            '-t',
            '14'
        ]
    );

    fs.writeFileSync(`./data_policy/samples_depth_${Egaroucid_DEPTH}_${top}.txt`, stdout.toString(), 'utf8')
    fs.writeFileSync('./data_policy/top.txt', top.toString(), 'utf8')
}

for(let i = 0 ; i < 30 ; i++){
    generate_policy_sample()
}

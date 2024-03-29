const fs = require('fs').promises;
const git = require('@npmcli/git');
const util = require('util');
const cp = require('child_process');
const path = require('path');

const execFile = util.promisify(cp.execFile);

const CLONE_PATH = 'libcryxml';

async function main() {
    try {
        try {
            await git.clone('https://github.com/TanninOne/libcryxml', 'master', CLONE_PATH, {});
        } catch (err) {
            // an error with code 128 is emitted when the output directory already exists
            if (err.code !== 128) {
                throw err;
            }
        }

        const buildPath = path.join(CLONE_PATH, 'build');
        try {
            await fs.mkdir(buildPath);
        } catch (err) {
            if (err.code !== 'EEXIST') {
                throw err;
            }
        }
        await execFile('cmake', ['..', '-DCMAKE_GENERATOR_PLATFORM=x64'], { cwd: path.join(__dirname, buildPath) });
        await execFile('cmake', ['--build', '.', '--config', 'Release'], { cwd: path.join(__dirname, buildPath) });
        await execFile('cmake', ['--install', '.'], { cwd: path.join(__dirname, buildPath) });
    } catch (err) {
        console.error(err);
    }
}

main();

import * as cp from 'crypak';
import * as cx from 'cryxml';
import * as fs from 'fs';
import * as path from 'path';

export function main(pakPath: string, keyPath: string) {
    const wolcenkey = fs.readFileSync(keyPath).toString('hex');

    // list all files in the Umbra.pak file
    const files = cp.listFiles(pakPath, wolcenkey);
    console.log(`pak contains ${files.length} files`);

    // pick a random xml file from the pak
    const xmlFiles = files.filter(name => path.extname(name) === '.xml');
    const pos = Math.floor(Math.random() * xmlFiles.length);
    console.log('picking', xmlFiles[pos]);
    const binBuf = cp.decryptFiles(pakPath, wolcenkey, xmlFiles.slice(pos, pos + 1));

    // assuming files aren't compressed, ignoring the header completely
    const offset = String.fromCharCode(...binBuf[0]).indexOf('CryXmlB');
    const data = binBuf[0].slice(offset);

    // decode the binary xml
    const xml = cx.decodeBuffer(data);
    console.log(xml);
}

if (require.main === module) {
    main(process.argv[2], process.argv[3]);
}

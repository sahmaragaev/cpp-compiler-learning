const vscode = require('vscode');

function activate(context) {
    console.log('Nova language extension is now active!');
    
    let runCommand = vscode.commands.registerCommand('nova.run', () => {
        const editor = vscode.window.activeTextEditor;
        if (editor && editor.document.languageId === 'nova') {
            const filePath = editor.document.fileName;
            const terminal = vscode.window.createTerminal('Nova');
            terminal.show();
            terminal.sendText(`nova "${filePath}"`);
        }
    });
    
    context.subscriptions.push(runCommand);
}

function deactivate() {}

module.exports = {
    activate,
    deactivate
}
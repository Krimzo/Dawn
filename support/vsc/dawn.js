const vscode = require("vscode");

function activate(context)
{
  let runDawn = vscode.commands.registerCommand("dawn.runFile", () => {
    const editor = vscode.window.activeTextEditor;
    if (!editor)
      return;

    let terminal = vscode.window.activeTerminal;
    if (!terminal)
      terminal = vscode.window.createTerminal("Dawn");
    
    terminal.show();
    terminal.sendText(`dawn "${editor.document.fileName}"`);
    editor.show();
  });
  context.subscriptions.push(runDawn);
}

function deactivate()
{}

module.exports = { activate, deactivate };

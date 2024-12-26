// Modules to control application life and create native browser window
const { app, BrowserWindow, ipcMain } = require('electron')
const path = require('node:path')
const obsaddon = require('./build/Release/obsaddon');
const { dialog } = require('electron');

function createWindow () {
  // Create the browser window.
  const mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      contextIsolation: true,
      nodeIntegration: true
    }
  })

  // and load the index.html of the app.
  mainWindow.loadFile('index.html')

  // Open the DevTools.
  //mainWindow.webContents.openDevTools()
}

function showMessage(msg) {
  dialog.showMessageBox({
    type: 'info',
    title: 'MessageBox',
    message: msg
  });
}

ipcMain.on("check-startup-button-clicked", (event, data) => {
  showMessage("OBS startup ok?: " + obsaddon.test_obs_startup());
} )

ipcMain.on("version-button-clicked", (event, data) => {
  showMessage("OBS version: " + obsaddon.obs_version());
} )

ipcMain.on("check-init-button-clicked", (event, data) => {
  showMessage("OBS initialized: " + obsaddon.obs_initialized());
} )

ipcMain.on("start-button-clicked", (event, data) => {
  showMessage("Start streaming: " + obsaddon.start_stream());
} )

ipcMain.on("stop-button-clicked", (event, data) => {
  showMessage("Stop streaming: " + obsaddon.stop_stream());
} )


// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.whenReady().then(() => {
  createWindow()

  app.on('activate', function () {
    // On macOS it's common to re-create a window in the app when the
    // dock icon is clicked and there are no other windows open.
    if (BrowserWindow.getAllWindows().length === 0) createWindow()
  })
})

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', function () {
  if (process.platform !== 'darwin') app.quit()
})

// In this file you can include the rest of your app's specific main process
// code. You can also put them in separate files and require them here.

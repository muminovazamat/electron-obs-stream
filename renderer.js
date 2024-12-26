/**
 * This file is loaded via the <script> tag in the index.html file and will
 * be executed in the renderer process for that window. No Node.js APIs are
 * available in this process because `nodeIntegration` is turned off and
 * `contextIsolation` is turned on. Use the contextBridge API in `preload.js`
 * to expose Node.js functionality from the main process.
 */

const checkStartupButton = document.getElementById("check-startup")
checkStartupButton.onclick = function () {
    window.electron.send("check-startup-button-clicked", {})
}

const versionButton = document.getElementById("show-obs-version")
versionButton.onclick = function () {
    window.electron.send("version-button-clicked", {})
}

const checkInitButton = document.getElementById("check-init")
checkInitButton.onclick = function () {
    window.electron.send("check-init-button-clicked", {})
}

const startButton = document.getElementById("start-stream")
startButton.onclick = function () {
    window.electron.send("start-button-clicked", {})
}

const stopButton = document.getElementById("stop-stream")
stopButton.onclick = function () {
    window.electron.send("stop-button-clicked", {})
}
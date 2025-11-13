function initTheme() {
    const prefersDarkScheme = window.matchMedia("(prefers-color-scheme: dark)").matches;
    if (prefersDarkScheme) {
        document.body.classList.remove("light");
        document.body.classList.add("dark");

    } else {
        document.body.classList.add("light");
        document.body.classList.remove("dark");
    }
}

initTheme();

// Use a broadcast channel to tell other ref-tests instances to stop
const id = Math.random().toString(36).substring(7);
const channel = new BroadcastChannel('reftest');
channel.onmessage = (event) => {
    if (event.data.id !== id && event.data.msg === 'stop') {
        window.close();
    }
}
channel.postMessage({from: id, msg: 'stop'});

function initTheme() {
    const saved = localStorage.getItem('reftest-theme');
    const systemDark = window.matchMedia('(prefers-color-scheme: dark)');

    const applyTheme = (isDark) => {
        document.body.classList.toggle('dark', isDark);
        document.body.classList.toggle('light', !isDark);
    };

    if (saved) {
        applyTheme(saved === 'dark');
    } else {
        applyTheme(systemDark.matches);
        // Listen for system changes only if no user override exists
        systemDark.addEventListener('change', (e) => {
            if (!localStorage.getItem('reftest-theme')) {
                applyTheme(e.matches);
            }
        });
    }
}
initTheme();

function toggleTheme() {
    const isDark = document.body.classList.contains('dark');
    const newTheme = isDark ? 'light' : 'dark';

    document.body.classList.remove('light', 'dark');
    document.body.classList.add(newTheme);
    localStorage.setItem('reftest-theme', newTheme);
}

function loadLazyElements(container) {
    if (container.dataset.resourcesLoaded) return;

    container.querySelectorAll('img[data-src]').forEach(el => {
        el.src = el.dataset.src;
        el.removeAttribute('data-src');
    });

    container.querySelectorAll('.iframe-placeholder[data-src]').forEach(placeholder => {
        const iframe = document.createElement('iframe');
        iframe.src = placeholder.dataset.src;
        iframe.style.cssText = placeholder.style.cssText + 'background-color: white; border: none;';
        placeholder.replaceWith(iframe);
    });

    container.dataset.resourcesLoaded = 'true';
}

function initLazyLoading() {
    document.addEventListener('toggle', (event) => {
        if (event.target.tagName === 'DETAILS' && event.target.open) {
            loadLazyElements(event.target);
        }
    }, true);

    document.querySelectorAll('details[open]').forEach(loadLazyElements);
}


document.addEventListener('DOMContentLoaded', () => {
    initLazyLoading();
});

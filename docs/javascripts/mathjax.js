window.MathJax = {
  tex: {
    inlineMath: [["\\(", "\\)"]],
    displayMath: [["\\[", "\\]"]],
    processEscapes: true,
    processEnvironments: true
  },
  options: {
    ignoreHtmlClass: ".*|",
    processHtmlClass: "arithmatex"
  }
};

// Only a few pages use math, so load MathJax (~270 KB gzipped, from jsDelivr)
// on the first page that has arithmatex output instead of on every page. Its
// startup typesets whatever page is current when it finishes loading.
const MATHJAX_SRC = "https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-mml-chtml.js";
let mathjaxRequested = false;

// Runs on the first page load and after each instant navigation.
document$.subscribe(() => {
  if (!document.querySelector(".arithmatex")) return;

  if (!mathjaxRequested) {
    mathjaxRequested = true;
    const script = document.createElement("script");
    script.src = MATHJAX_SRC;
    script.async = true;
    document.head.appendChild(script);
    return;
  }

  // Still downloading: window.MathJax is the config object until the bundle
  // runs, and startup will typeset the current page.
  if (!MathJax.startup) return;

  // Loaded: re-typeset the swapped-in page content.
  MathJax.startup.promise.then(() => {
    MathJax.startup.output.clearCache();
    MathJax.typesetClear();
    MathJax.texReset();
    return MathJax.typesetPromise();
  });
});

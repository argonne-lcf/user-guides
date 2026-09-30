// content.tabs.link matches tab labels by innerText, which is empty inside a
// closed <details>, so tab sets in collapsed ??? blocks never pick up the
// linked selection. Apply the stored selection when a <details> opens.
document$.subscribe(function () {
  document.querySelectorAll("article details").forEach(function (details) {
    details.addEventListener("toggle", function () {
      if (!details.open) return
      var tabs = __md_get("__tabs") || []
      details.querySelectorAll(".tabbed-set").forEach(function (set) {
        var labels = Array.from(set.querySelectorAll(":scope > .tabbed-labels > label"))
        for (var i = 0; i < tabs.length; i++) {
          var label = labels.find(function (l) { return l.textContent.trim() === tabs[i] })
          if (label) {
            var input = document.getElementById(label.htmlFor)
            if (!input.checked) {
              // Stops Material from re-linking every other set and re-sorting storage.
              label.setAttribute("data-md-switching", "")
              input.click()
            }
            break
          }
        }
      })
    })
  })
})

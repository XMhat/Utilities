// Reload styles
function reloadStyles() {
  // Remove current styles
  safari.extension.removeContentStyleSheets();
  // Is deactivated? Ignore
  if(!safari.extension.settings.activated) return;
  // Add new stylesheet
  safari.extension.addContentStyleSheet(
    "* {background-color:"+safari.extension.settings.bg+" !important;"+
    "color:"+safari.extension.settings.fg+" !important;"+
    "text-shadow:0 0 0 #000 !important;"+
    "cursor:auto !important;"+
    "border-color:"+safari.extension.settings.border+" !important;"+
    "outline-color:"+safari.extension.settings.border+" !important;}"+
    "a {color:"+safari.extension.settings.link+" !important;}"+
    "a:hover {color:"+safari.extension.settings.linkh+" !important;}"+
    "a:visited {color:"+safari.extension.settings.linkv+" !important;}");
}
// Register command event
safari.application.addEventListener('command', function(event) {
  // Was not toggle toolbar button?
  if(event.command != 'toggle') return;
  // Toggle setting
  safari.extension.settings.activated ^= true;
  // Reload styles
  reloadStyles();
}, false);
// Immediately set style
reloadStyles();

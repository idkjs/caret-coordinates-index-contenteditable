function getCaretCoordinates() {
  let x = 0,
    y = 0;
  const isSupported = typeof window.getSelection !== 'undefined';
  if (isSupported) {
    const selection = window.getSelection();
    console.log('getCaretCoordinates selection:', selection);
    // Check if there is a selection (i.e. cursor in place)
    if (selection.rangeCount !== 0) {
      // Clone the range
      const range = selection.getRangeAt(0).cloneRange();
      console.log('getCaretCoordinates rangeClone:', range);
      // Collapse the range to the start, so there are not multiple chars selected
      range.collapse(true);
      // getCientRects returns all the positioning information we need
      const rect = range.getClientRects()[0];
      console.log('getCaretCoordinates rect:', rect);
      if (rect) {
        x = rect.left; // since the caret is only 1px wide, left == right
        y = rect.top; // top edge of the caret
      }
    }
  }
  return { x, y };
}

function getCaretIndex(element) {
  let position = 0;
  const isSupported = typeof window.getSelection !== 'undefined';
  if (isSupported) {
    const selection = window.getSelection();
    // Check if there is a selection (i.e. cursor in place)
    if (selection.rangeCount !== 0) {
      // Store the original range
      const range = window.getSelection().getRangeAt(0);
      console.log('original range', range);
      // Clone the range
      const preCaretRange = range.cloneRange();
      console.log('preCaretRange JS: ', preCaretRange);
      // Select all textual contents from the contenteditable element
      preCaretRange.selectNodeContents(element);
      console.log('preCaretRange  selectNodeContents JS: ', preCaretRange.selectNodeContents(element));
      // And set the range end to the original clicked position
      preCaretRange.setEnd(range.endContainer, range.endOffset);
      console.log('preCaretRange  setEnd JS: ', preCaretRange);

      // Return the text length from contenteditable start to the range end
      position = preCaretRange.toString().length;
      console.log('preCaretRange  position JS: ', position);
    }
  }
  return position;
}
// const { x, y } = getCaretCoordinates();
// console.log(`display: inline-block; left: ${x - 32}px; top: ${y - 36}px`);

function toggleTooltip(event, contenteditable) {
  const tooltip = document.getElementById('tooltip');
  console.log('ToggleTooltip event:', event);
  console.log('ToggleTooltip event.target:', event.target);
  console.log('contenteditable.contains(event.target):', contenteditable.contains(event.target));
  if (contenteditable.contains(event.target)) {
    const { x, y } = getCaretCoordinates();
      console.log('ToggleTooltip getCaretCoordinates:', x, y );
    tooltip.setAttribute('aria-hidden', 'false');
    tooltip.setAttribute('style', `display: inline-block; left: ${x - 32}px; top: ${y - 36}px`);
  } else {
    tooltip.setAttribute('aria-hidden', 'true');
    tooltip.setAttribute('style', 'display: none;');
  }
}

function updateIndex(event, element) {
  const textPosition = document.getElementById('caretIndex');
  if (element.contains(event.target)) {
    textPosition.innerText = getCaretIndex(element).toString();
  } else {
    textPosition.innerText = '–';
  }
}

const editable = document.getElementById('contenteditable');
document.addEventListener('click', (e) => toggleTooltip(e, editable));
document.addEventListener('click', (e) => updateIndex(e, editable));
document.addEventListener('keyup', (e) => toggleTooltip(e, editable));
document.addEventListener('keyup', (e) => updateIndex(e, editable));

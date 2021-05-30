open Webapi.Dom;

let isSupported = () =>
  Js.typeof(Webapi.Dom.Window.getSelection) !== "undefined";
let getSelection = () => window->Window.getSelection;
let getRangeCount = selection => selection |> Webapi.Dom.Selection.rangeCount;
let rangeZero = selection => selection |> Selection.getRangeAt(0);
let getCaretCoordinates = () => {
  let left = 0.;
  let top = 0.;

  isSupported()
    ? {
      let selection = getSelection();
      let rangeCount = getRangeCount(selection);

      // Check if there is a selection (i.e. cursor in place)
      rangeCount !== 0
        // Clone the range
        ? {
          let range = Range.cloneRange(rangeZero(selection));

          // Collapse the range to the start, so there are not multiple chars selected
          range |> Range.collapse;
          // getCientRects returns all the positioning information we need
          let rect = range |> Range.getClientRects;
          Js.log2("rect", rect);
          rect->Array.length > 0
            ? {
              let rect: Dom.domRect = rect |> Array.unsafe_get(_, 0);

              let left = rect->DomRect.left; // since the caret is only 1px wide, left == right
              let top = rect->DomRect.top; // top edge of the caret

              (left, top);
            }
            : (left, top);
        }
        : (left, top);
    }
    : (left, top);
};
let getCaretIndex = element => {
  Js.log("getCaretIndex called");
  isSupported()
    ? {
      let selection = getSelection();
      let rangeCount = getRangeCount(selection);
      // Check if there is a selection (i.e. cursor in place)
      rangeCount !== 0
        // Clone the range
        ? {
          let range = selection->rangeZero;
          // Collapse the range to the start, so there are not multiple chars selected
          range |> Range.collapse;

          //  Clone the range
          let preCaretRange = range |> Range.cloneRange;
          // Select all textual contents from the contenteditable element
          // Im not understanding why we do this. It always comes back undefined.
          let selectNodeContents =
            preCaretRange |> Range.selectNodeContents(element);
          Js.log2("selectNodeContents: Im not understanding why we do this. It always comes back: ", selectNodeContents);
          preCaretRange |> Range.selectNodeContents(element);
          // And set the range end to the original clicked position
          preCaretRange
          |> Range.setEnd(
               range |> Range.endContainer,
               range |> Range.endOffset,
             );
          // Return the text length from contenteditable start to the range end
          let position = preCaretRange |> Range.toString |> Js.String.length;

          position;
        }
        : 0;
    }
    : 0;
};
let getLeft = () => fst(getCaretCoordinates());
let getTop = () => snd(getCaretCoordinates());
// these next to have to use unit in order to the get the value at call time or it wont work.
let leftStr = () => Js.Float.toString(getLeft() -. 36.);
let topStr = () => Js.Float.toString(getTop() -. 36.);

let getTooltip = () =>
  switch (document |> Document.getElementById("tooltip")) {
  | None => raise(Invalid_argument("getFormElement failed"))
  | Some(el) => el
  };
let selectTooltip = () => {
  let tooltip = getTooltip();
  Element.setAttribute("aria-hidden", "false", tooltip);
  Element.setAttribute(
    "style",
    "display: inline-block; left: "
    ++ leftStr()
    ++ "px; top: "
    ++ topStr()
    ++ "px",
    tooltip,
  );
  ();
};
let elementContains = (event, element) => {
  let event = Obj.magic(event);
  let eventTarget = MouseEvent.target(event) |> EventTarget.unsafeAsElement;
  element |> Element.contains(eventTarget);
};

let toggleTooltip = (event, contenteditable) => {
  (
    elementContains(event, contenteditable)
      ? {
        Document.getElementById("tooltip", document)
        ->Belt.Option.map(_, Element.setAttribute("aria-hidden", "false"))
        ->ignore;
        Document.getElementById("tooltip", document)
        ->Belt.Option.map(
            _,
            Element.setAttribute(
              "style",
              "display: inline-block; left: "
              ++ leftStr()
              ++ "px; top: "
              ++ topStr()
              ++ "px",
            ),
          );
      }
      : {
        Document.getElementById("tooltip", document)
        ->Belt.Option.map(_, Element.setAttribute("aria-hidden", "true"))
        ->ignore;
        Document.getElementById("tooltip", document)
        ->Belt.Option.map(
            _,
            Element.setAttribute("style", "display: none;"),
          );
      }
  )
  |> ignore;
};

let setInnerText = (text: string, elementId: string): option(unit) =>
  Document.getElementById(elementId, document)
  ->Belt.Option.map(_, Element.setInnerText(_, text));

let updateIndex = (event, element) => {
  Js.log("updateIndex called");

  let caretIndex = getCaretIndex(element) |> string_of_int;

  (
    elementContains(event, element)
      ? {
        setInnerText(caretIndex, "caretIndex");
      }
      : {
        setInnerText(caretIndex, "–");
      }
  )
  |> ignore;
};

let editable = document |> Document.getElementById("contenteditable");

switch (editable) {
| Some(element) =>
  document
  |> Document.addEventListener("click", event =>
       toggleTooltip(event, element)
     );
  document
  |> Document.addEventListener("click", event => updateIndex(event, element));
  document
  |> Document.addEventListener("keyup", event =>
       toggleTooltip(event, element)
     );
  document
  |> Document.addEventListener("keyup", event => updateIndex(event, element));

| None => Js.log("editable not found")
};

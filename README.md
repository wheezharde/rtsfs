# rtsfs
RTS From Scratch

Just a fun little project making an RTS from scratch. We'll see how far it goes. :)

The intent is to write pretty much everything, but often these will just be an interface to reused libraries.  For example, the project may use some modern C++ STL, but it'll be wrapped or typedef'd appropriately.

The project is a mixture of C and C+ where objects are typically POD or abstract. "Modern C++" will be mostly ignored for simplicity of reading ("just because YOU know what it does doesn't mean that ALL programmers do; K.I.S.")

My style rules for coding are reasonably simple (imho):
  - I prefer spaces for manual alignment of text (diagrams, comments, spacing, etc.) as tabs vary in size.
  - counts of things which can't reasonably be negative are in size_t.
  - horizontal whitespace is helpful, but wrap at half a "standard" monitor at "reasonable" text size (120 chars.)
  - vertical whitespace is precious, so opening braces are on the same line as the opening statement.
  - I typically do camel case as it's what my job does (switching for personal projects is hard!) but I use lowerspace names for things.
  - respect the C reservations (*_t, single and double underscore leading characters)

Code will be "mostly" to "somewhat" platform agnostic. Library/Core code should be "mostly" platform agnostic as well. I do not intend to create a pure platform agnostic codebase as that's been the death knell of many-a-personal-project already.


# immutable_string
Tools about strings

# Contains
- The starting point: nativa::string_view
  - Representing a continuous sequence of `char`s.
  - Immutable
  - Does not own the string
  - Encoding-free
  - May not be seen as C-style strings
  - Can be trivially copied
- The center of the stage: nativa::string
  - Derived from nativa::string_view, thus sharing most of the features
  - Owns the string
  - Reference counted
  - Guaranteed to be used as C-style strings
- Others
  - nativa::format
    - Basic implementation for a string formatter with .NET-like syntax
  - nativa::encoding::utf8
    - Provides an iterator and a wrapper-container to access a string's chars as if it was encoded in UTF-8
  - nativa::string_builder and nativa::fixed_string_builder
    - As their name implies. The fixed one uses the stack memory and is thus a little bit faster.

# Usage
Basically, you can know the usage from the header files. The only problem is that not all the document is in them and some of them (especially the header-only ones) are horribly written. If anybody **ever uses** this thing, I would appreciate it very much and I will soon fill up this part. Thank you very much.

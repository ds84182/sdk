//
// THIS FILE IS GENERATED. DO NOT EDIT.
//
// Instead modify 'pkg/front_end/messages.yaml' and run
// 'dart pkg/analyzer/tool/messages/generate.dart' to update.

part of 'syntactic_errors.dart';

final fastaAnalyzerErrorCodes = <ErrorCode>[
  null,
  _EQUALITY_CANNOT_BE_EQUALITY_OPERAND,
  _CONTINUE_OUTSIDE_OF_LOOP,
  _EXTERNAL_CLASS,
  _STATIC_CONSTRUCTOR,
  _EXTERNAL_ENUM,
  _PREFIX_AFTER_COMBINATOR,
  _TYPEDEF_IN_CLASS,
  _COVARIANT_AFTER_VAR,
  _INVALID_AWAIT_IN_FOR,
  _IMPORT_DIRECTIVE_AFTER_PART_DIRECTIVE,
];

const ParserErrorCode _CONTINUE_OUTSIDE_OF_LOOP = const ParserErrorCode(
    'CONTINUE_OUTSIDE_OF_LOOP',
    "A continue statement can't be used outside of a loop or switch statement.",
    correction: "Try removing the continue statement.");

const ParserErrorCode _COVARIANT_AFTER_VAR = const ParserErrorCode(
    'COVARIANT_AFTER_VAR',
    "The modifier 'covariant' should be before the modifier 'var'.",
    correction: "Try re-ordering the modifiers.");

const ParserErrorCode _EQUALITY_CANNOT_BE_EQUALITY_OPERAND = const ParserErrorCode(
    'EQUALITY_CANNOT_BE_EQUALITY_OPERAND',
    "An equality expression can't be an operand of another equality expression.",
    correction: "Try re-writing the expression.");

const ParserErrorCode _EXTERNAL_CLASS = const ParserErrorCode(
    'EXTERNAL_CLASS', "Classes can't be declared to be 'external'.",
    correction: "Try removing the keyword 'external'.");

const ParserErrorCode _EXTERNAL_ENUM = const ParserErrorCode(
    'EXTERNAL_ENUM', "Enums can't be declared to be 'external'.",
    correction: "Try removing the keyword 'external'.");

const ParserErrorCode _IMPORT_DIRECTIVE_AFTER_PART_DIRECTIVE =
    const ParserErrorCode('IMPORT_DIRECTIVE_AFTER_PART_DIRECTIVE',
        "Import directives must preceed part directives.",
        correction:
            "Try moving the import directives before the part directives.");

const ParserErrorCode _INVALID_AWAIT_IN_FOR = const ParserErrorCode(
    'INVALID_AWAIT_IN_FOR',
    "The keyword 'await' isn't allowed for a normal 'for' statement.",
    correction: "Try removing the keyword, or use a for-each statement.");

const ParserErrorCode _PREFIX_AFTER_COMBINATOR = const ParserErrorCode(
    'PREFIX_AFTER_COMBINATOR',
    "The prefix ('as' clause) should come before any show/hide combinators.",
    correction: "Try moving the prefix before the combinators.");

const ParserErrorCode _STATIC_CONSTRUCTOR = const ParserErrorCode(
    'STATIC_CONSTRUCTOR', "Constructors can't be static.",
    correction: "Try removing the keyword 'static'.");

const ParserErrorCode _TYPEDEF_IN_CLASS = const ParserErrorCode(
    'TYPEDEF_IN_CLASS', "Typedefs can't be declared inside classes.",
    correction: "Try moving the typedef to the top-level.");
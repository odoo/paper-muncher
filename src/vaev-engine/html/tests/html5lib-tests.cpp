// Tests imported from https://github.com/html5lib/html5lib-tests
//
// Copyright (c) 2006-2013 James Graham, Geoffrey Sneddon, and
// other contributors
// 
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <karm/test>

import Karm.Core;

import Html5LibTest;

using namespace Karm;

namespace Vaev::Html::Tests {

test$("html5lib-adoption01-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<a><p></a></p>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,10): adoption-agency-1.3\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <a>\n"
        "|     <p>\n"
        "|       <a>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-adoption01-1") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<a>1<p>2</a>3</p>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,12): adoption-agency-1.3\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <a>\n"
        "|       \"1\"\n"
        "|     <p>\n"
        "|       <a>\n"
        "|         \"2\"\n"
        "|       \"3\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-adoption01-2") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<a>1<button>2</a>3</button>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,17): adoption-agency-1.3\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <a>\n"
        "|       \"1\"\n"
        "|     <button>\n"
        "|       <a>\n"
        "|         \"2\"\n"
        "|       \"3\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-adoption01-3") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<a>1<b>2</a>3</b>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,12): adoption-agency-1.3\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <a>\n"
        "|       \"1\"\n"
        "|       <b>\n"
        "|         \"2\"\n"
        "|     <b>\n"
        "|       \"3\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-adoption01-4") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<a>1<div>2<div>3</a>4</div>5</div>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,20): adoption-agency-1.3\n"
        "(1,20): adoption-agency-1.3\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <a>\n"
        "|       \"1\"\n"
        "|     <div>\n"
        "|       <a>\n"
        "|         \"2\"\n"
        "|       <div>\n"
        "|         <a>\n"
        "|           \"3\"\n"
        "|         \"4\"\n"
        "|       \"5\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-adoption01-6") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<b><b><a><p></a>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,16): adoption-agency-1.3\n"
        "(1,16): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <b>\n"
        "|       <b>\n"
        "|         <a>\n"
        "|         <p>\n"
        "|           <a>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-adoption01-7") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<b><a><b><p></a>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,16): adoption-agency-1.3\n"
        "(1,16): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <b>\n"
        "|       <a>\n"
        "|         <b>\n"
        "|       <b>\n"
        "|         <p>\n"
        "|           <a>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-adoption01-8") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<a><b><b><p></a>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,16): adoption-agency-1.3\n"
        "(1,16): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <a>\n"
        "|       <b>\n"
        "|         <b>\n"
        "|     <b>\n"
        "|       <b>\n"
        "|         <p>\n"
        "|           <a>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-adoption01-9") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<p>1<s id=\"A\">2<b id=\"B\">3</p>4</s>5</b>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,30): unexpected-end-tag\n"
        "(1,35): adoption-agency-1.3\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <p>\n"
        "|       \"1\"\n"
        "|       <s>\n"
        "|         id=\"A\"\n"
        "|         \"2\"\n"
        "|         <b>\n"
        "|           id=\"B\"\n"
        "|           \"3\"\n"
        "|     <s>\n"
        "|       id=\"A\"\n"
        "|       <b>\n"
        "|         id=\"B\"\n"
        "|         \"4\"\n"
        "|     <b>\n"
        "|       id=\"B\"\n"
        "|       \"5\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-adoption01-11") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table>A<td>B</td>C</table>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,8): unexpected-character-implies-table-voodoo\n"
        "(1,12): unexpected-cell-in-table-body\n"
        "(1,22): unexpected-character-implies-table-voodoo\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"AC\"\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|           <td>\n"
        "|             \"B\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-adoption01-13") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div><a><b><div><div><div><div><div><div><div><div><div><div></a>\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,65): adoption-agency-1.3\n"
        "(1,65): adoption-agency-1.3\n"
        "(1,65): adoption-agency-1.3\n"
        "(1,65): adoption-agency-1.3\n"
        "(1,65): adoption-agency-1.3\n"
        "(1,65): adoption-agency-1.3\n"
        "(1,65): adoption-agency-1.3\n"
        "(1,65): adoption-agency-1.3\n"
        "(1,65): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       <a>\n"
        "|         <b>\n"
        "|       <b>\n"
        "|         <div>\n"
        "|           <a>\n"
        "|           <div>\n"
        "|             <a>\n"
        "|             <div>\n"
        "|               <a>\n"
        "|               <div>\n"
        "|                 <a>\n"
        "|                 <div>\n"
        "|                   <a>\n"
        "|                   <div>\n"
        "|                     <a>\n"
        "|                     <div>\n"
        "|                       <a>\n"
        "|                       <div>\n"
        "|                         <a>\n"
        "|                           <div>\n"
        "|                             <div>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-adoption01-14") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div><a><b><u><i><code><div></a>\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,32): adoption-agency-1.3\n"
        "(1,32): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       <a>\n"
        "|         <b>\n"
        "|           <u>\n"
        "|             <i>\n"
        "|               <code>\n"
        "|       <u>\n"
        "|         <i>\n"
        "|           <code>\n"
        "|             <div>\n"
        "|               <a>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-adoption01-15") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<b><b><b><b>x</b></b></b></b>y\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <b>\n"
        "|       <b>\n"
        "|         <b>\n"
        "|           <b>\n"
        "|             \"x\"\n"
        "|     \"y\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-adoption01-16") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<p><b><b><b><b><p>x\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,18): unexpected-end-tag\n"
        "(1,19): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <p>\n"
        "|       <b>\n"
        "|         <b>\n"
        "|           <b>\n"
        "|             <b>\n"
        "|     <p>\n"
        "|       <b>\n"
        "|         <b>\n"
        "|           <b>\n"
        "|             \"x\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-adoption02-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<b>1<i>2<p>3</b>4\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,16): adoption-agency-1.3\n"
        "(1,17): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <b>\n"
        "|       \"1\"\n"
        "|       <i>\n"
        "|         \"2\"\n"
        "|     <i>\n"
        "|       <p>\n"
        "|         <b>\n"
        "|           \"3\"\n"
        "|         \"4\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-comments01-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO<!-- BAR -->BAZ\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO\"\n"
        "|     <!--  BAR  -->\n"
        "|     \"BAZ\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-comments01-5") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO<!-- BAR -- <QUX> -- MUX -->BAZ\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO\"\n"
        "|     <!--  BAR -- <QUX> -- MUX  -->\n"
        "|     \"BAZ\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-comments01-8") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO<!---->BAZ\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO\"\n"
        "|     <!--  -->\n"
        "|     \"BAZ\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-comments01-9") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO<!--->BAZ\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "(1,9): incorrect-comment\n"
        "#new-errors\n"
        "(1:9) abrupt-closing-of-empty-comment\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO\"\n"
        "|     <!--  -->\n"
        "|     \"BAZ\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-comments01-10") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO<!-->BAZ\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "(1,8): incorrect-comment\n"
        "#new-errors\n"
        "(1:8) abrupt-closing-of-empty-comment\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO\"\n"
        "|     <!--  -->\n"
        "|     \"BAZ\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-comments01-11") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<?xml version=\"1.0\">Hi\n"
        "#errors\n"
        "(1,1): expected-tag-name-but-got-question-mark\n"
        "(1,22): expected-doctype-but-got-chars\n"
        "#new-errors\n"
        "(1:2) unexpected-question-mark-instead-of-tag-name\n"
        "#document\n"
        "| <!-- ?xml version=\"1.0\" -->\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"Hi\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-comments01-12") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<?xml version=\"1.0\">\n"
        "#errors\n"
        "(1,1): expected-tag-name-but-got-question-mark\n"
        "(1,20): expected-doctype-but-got-eof\n"
        "#new-errors\n"
        "(1:2) unexpected-question-mark-instead-of-tag-name\n"
        "#document\n"
        "| <!-- ?xml version=\"1.0\" -->\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-comments01-14") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO<!----->BAZ\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO\"\n"
        "|     <!-- - -->\n"
        "|     \"BAZ\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-doctype01-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html>Hello\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"Hello\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-doctype01-2") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPEhtml>Hello\n"
        "#errors\n"
        "(1,9): need-space-after-doctype\n"
        "#new-errors\n"
        "(1:10) missing-whitespace-before-doctype-name\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"Hello\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-doctype01-3") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE>Hello\n"
        "#errors\n"
        "(1,10): expected-doctype-name-but-got-right-bracket\n"
        "(1,10): unknown-doctype\n"
        "#new-errors\n"
        "(1:10) missing-doctype-name\n"
        "#document\n"
        "| <!DOCTYPE >\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"Hello\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-doctype01-4") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE >Hello\n"
        "#errors\n"
        "(1,11): expected-doctype-name-but-got-right-bracket\n"
        "(1,11): unknown-doctype\n"
        "#new-errors\n"
        "(1:11) missing-doctype-name\n"
        "#document\n"
        "| <!DOCTYPE >\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"Hello\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-doctype01-5") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE potato>Hello\n"
        "#errors\n"
        "(1,17): unknown-doctype\n"
        "#document\n"
        "| <!DOCTYPE potato>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"Hello\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-doctype01-6") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE potato >Hello\n"
        "#errors\n"
        "(1,18): unknown-doctype\n"
        "#document\n"
        "| <!DOCTYPE potato>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"Hello\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-doctype01-7") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE potato taco>Hello\n"
        "#errors\n"
        "(1,17): expected-space-or-right-bracket-in-doctype\n"
        "(1,22): unknown-doctype\n"
        "#new-errors\n"
        "(1:18) invalid-character-sequence-after-doctype-name\n"
        "#document\n"
        "| <!DOCTYPE potato>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"Hello\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-doctype01-8") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE potato taco \"ddd>Hello\n"
        "#errors\n"
        "(1,17): expected-space-or-right-bracket-in-doctype\n"
        "(1,27): unknown-doctype\n"
        "#new-errors\n"
        "(1:18) invalid-character-sequence-after-doctype-name\n"
        "#document\n"
        "| <!DOCTYPE potato>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"Hello\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-doctype01-9") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE potato sYstEM>Hello\n"
        "#errors\n"
        "(1,24): unexpected-char-in-doctype\n"
        "(1,24): unknown-doctype\n"
        "#new-errors\n"
        "(1:24) missing-doctype-system-identifier\n"
        "#document\n"
        "| <!DOCTYPE potato>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"Hello\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-doctype01-10") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE potato sYstEM    >Hello\n"
        "#errors\n"
        "(1,28): unexpected-char-in-doctype\n"
        "(1,28): unknown-doctype\n"
        "#new-errors\n"
        "(1:28) missing-doctype-system-identifier\n"
        "#document\n"
        "| <!DOCTYPE potato>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"Hello\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-doctype01-11") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE   potato       sYstEM  ggg>Hello\n"
        "#errors\n"
        "(1,34): unexpected-char-in-doctype\n"
        "(1,37): unknown-doctype\n"
        "#new-errors\n"
        "(1:34) missing-quote-before-doctype-system-identifier\n"
        "#document\n"
        "| <!DOCTYPE potato>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"Hello\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-doctype01-12") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE potato SYSTEM taco  >Hello\n"
        "#errors\n"
        "(1,25): unexpected-char-in-doctype\n"
        "(1,31): unknown-doctype\n"
        "#new-errors\n"
        "(1:25) missing-quote-before-doctype-system-identifier\n"
        "#document\n"
        "| <!DOCTYPE potato>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"Hello\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-doctype01-16") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE potato SYSTEMtaco \"ddd\">Hello\n"
        "#errors\n"
        "(1,24): unexpected-char-in-doctype\n"
        "(1,34): unknown-doctype\n"
        "#new-errors\n"
        "(1:24) missing-quote-before-doctype-system-identifier\n"
        "#document\n"
        "| <!DOCTYPE potato>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"Hello\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-doctype01-17") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE potato grass SYSTEM taco>Hello\n"
        "#errors\n"
        "(1,17): expected-space-or-right-bracket-in-doctype\n"
        "(1,35): unknown-doctype\n"
        "#new-errors\n"
        "(1:18) invalid-character-sequence-after-doctype-name\n"
        "#document\n"
        "| <!DOCTYPE potato>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"Hello\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-doctype01-18") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE potato pUbLIc>Hello\n"
        "#errors\n"
        "(1,24): unexpected-end-of-doctype\n"
        "(1,24): unknown-doctype\n"
        "#new-errors\n"
        "(1:24) missing-doctype-public-identifier\n"
        "#document\n"
        "| <!DOCTYPE potato>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"Hello\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-doctype01-19") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE potato pUbLIc >Hello\n"
        "#errors\n"
        "(1,25): unexpected-end-of-doctype\n"
        "(1,25): unknown-doctype\n"
        "#new-errors\n"
        "(1:25) missing-doctype-public-identifier\n"
        "#document\n"
        "| <!DOCTYPE potato>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"Hello\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-doctype01-20") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE potato pUbLIcgoof>Hello\n"
        "#errors\n"
        "(1,24): unexpected-char-in-doctype\n"
        "(1,28): unknown-doctype\n"
        "#new-errors\n"
        "(1:24) missing-quote-before-doctype-public-identifier\n"
        "#document\n"
        "| <!DOCTYPE potato>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"Hello\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-doctype01-21") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE potato PUBLIC goof>Hello\n"
        "#errors\n"
        "(1,25): unexpected-char-in-doctype\n"
        "(1,29): unknown-doctype\n"
        "#new-errors\n"
        "(1:25) missing-quote-before-doctype-public-identifier\n"
        "#document\n"
        "| <!DOCTYPE potato>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"Hello\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-domjs-unsafe-3") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<script>a='\000'</script>\n"
        "#errors\n"
        "(1,8): expected-doctype-but-got-start-tag\n"
        "(1,12): invalid-codepoint\n"
        "#new-errors\n"
        "(1:12) unexpected-null-character\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <script>\n"
        "|       \"a='\357\277\275'\"\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-domjs-unsafe-4") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<script type=\"data\"><!--\000</script>\n"
        "#errors\n"
        "(1,20): expected-doctype-but-got-start-tag\n"
        "(1,25): invalid-codepoint\n"
        "#new-errors\n"
        "(1:25) unexpected-null-character\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <script>\n"
        "|       type=\"data\"\n"
        "|       \"<!--\357\277\275\"\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-domjs-unsafe-5") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<script type=\"data\"><!--foo\000</script>\n"
        "#errors\n"
        "(1,20): expected-doctype-but-got-start-tag\n"
        "(1,28): invalid-codepoint\n"
        "#new-errors\n"
        "(1:28) unexpected-null-character\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <script>\n"
        "|       type=\"data\"\n"
        "|       \"<!--foo\357\277\275\"\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-domjs-unsafe-6") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<script type=\"data\"><!-- foo-\000</script>\n"
        "#errors\n"
        "(1,20): expected-doctype-but-got-start-tag\n"
        "(1,30): invalid-codepoint\n"
        "#new-errors\n"
        "(1:30) unexpected-null-character\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <script>\n"
        "|       type=\"data\"\n"
        "|       \"<!-- foo-\357\277\275\"\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-domjs-unsafe-9") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<script type=\"data\"><!-- foo-<</script>\n"
        "#errors\n"
        "(1,20): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <script>\n"
        "|       type=\"data\"\n"
        "|       \"<!-- foo-<\"\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-domjs-unsafe-11") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<script type=\"data\"><!-- foo-</SCRIPT>\n"
        "#errors\n"
        "(1,20): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <script>\n"
        "|       type=\"data\"\n"
        "|       \"<!-- foo-\"\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-domjs-unsafe-12") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<script type=\"data\"><!--<p></script>\n"
        "#errors\n"
        "(1,20): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <script>\n"
        "|       type=\"data\"\n"
        "|       \"<!--<p>\"\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-domjs-unsafe-21") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<script type=\"data\"></scrip/></script>\n"
        "#errors\n"
        "(1,20): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <script>\n"
        "|       type=\"data\"\n"
        "|       \"</scrip/>\"\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-domjs-unsafe-22") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<script type=\"data\"></scrip ></script>\n"
        "#errors\n"
        "(1,20): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <script>\n"
        "|       type=\"data\"\n"
        "|       \"</scrip >\"\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-domjs-unsafe-26") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><!DOCTYPE html>\n"
        "#errors\n"
        "(1,30): unexpected-doctype\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-domjs-unsafe-27") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html><!DOCTYPE html>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,21): unexpected-doctype\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-domjs-unsafe-28") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html><head><!DOCTYPE html></head>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,27): unexpected-doctype\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-domjs-unsafe-29") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html><head></head><!DOCTYPE html>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,34): unexpected-doctype\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-domjs-unsafe-30") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<body></body><!DOCTYPE html>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,28): unexpected-doctype\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-domjs-unsafe-31") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><!DOCTYPE html></table>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,22): unexpected-doctype\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-domjs-unsafe-32") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<select><!DOCTYPE html></select>\n"
        "#errors\n"
        "(1,8): expected-doctype-but-got-start-tag\n"
        "(1,23): unexpected-doctype\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <select>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-domjs-unsafe-33") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><colgroup><!DOCTYPE html></colgroup></table>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,32): unexpected-doctype\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <colgroup>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-domjs-unsafe-34") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><colgroup><!--test--></colgroup></table>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <colgroup>\n"
        "|         <!-- test -->\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-domjs-unsafe-35") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><colgroup><html></colgroup></table>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,23): non-html-root\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <colgroup>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-domjs-unsafe-36") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><colgroup> foo</colgroup></table>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,32): foster-parenting-character-in-table\n"
        "(1,32): foster-parenting-character-in-table\n"
        "(1,32): foster-parenting-character-in-table\n"
        "(1,32): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"foo\"\n"
        "|     <table>\n"
        "|       <colgroup>\n"
        "|         \" \"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-domjs-unsafe-37") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<select><!--test--></select>\n"
        "#errors\n"
        "(1,8): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <select>\n"
        "|       <!-- test -->\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-domjs-unsafe-38") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<select><html></select>\n"
        "#errors\n"
        "(1,8): expected-doctype-but-got-start-tag\n"
        "(1,14): non-html-root\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <select>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-domjs-unsafe-39") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<frameset><html></frameset>\n"
        "#errors\n"
        "(1,10): expected-doctype-but-got-start-tag\n"
        "(1,16): non-html-root\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <frameset>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-domjs-unsafe-40") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<frameset></frameset><html>\n"
        "#errors\n"
        "(1,10): expected-doctype-but-got-start-tag\n"
        "(1,27): non-html-root\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <frameset>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-domjs-unsafe-41") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<frameset></frameset><!DOCTYPE html>\n"
        "#errors\n"
        "(1,10): expected-doctype-but-got-start-tag\n"
        "(1,36): unexpected-doctype\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <frameset>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-domjs-unsafe-42") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html><body></body></html><!DOCTYPE html>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,41): unexpected-doctype\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities01-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO&gt;BAR\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO>BAR\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities01-1") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO&gtBAR\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "(1,6): named-entity-without-semicolon\n"
        "#new-errors\n"
        "(1:7) missing-semicolon-after-character-reference\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO>BAR\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities01-2") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO&gt BAR\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "(1,6): named-entity-without-semicolon\n"
        "#new-errors\n"
        "(1:7) missing-semicolon-after-character-reference\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO> BAR\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities01-3") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO&gt;;;BAR\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO>;;BAR\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities01-4") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "I'm &notit; I tell you\n"
        "#errors\n"
        "(1,4): expected-doctype-but-got-chars\n"
        "(1,9): named-entity-without-semicolon\n"
        "#new-errors\n"
        "(1:9) missing-semicolon-after-character-reference\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"I'm \302\254it; I tell you\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities01-5") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "I'm &notin; I tell you\n"
        "#errors\n"
        "(1,4): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"I'm \342\210\211 I tell you\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities01-6") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "&ammmp;\n"
        "#errors\n"
        "(1,1): expected-doctype-but-got-chars\n"
        "(1,7): unknown-named-character-reference\n"
        "#new-errors\n"
        "(1:7) unknown-named-character-reference\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"&ammmp;\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities01-7") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "&ammmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmp;\n"
        "#errors\n"
        "(1,1): expected-doctype-but-got-chars\n"
        "(1,950): unknown-named-character-reference\n"
        "#new-errors\n"
        "(1:950) unknown-named-character-reference\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"&ammmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmp;\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities01-8") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO& BAR\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO& BAR\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities01-9") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO&<BAR>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "(1,9): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO&\"\n"
        "|     <bar>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities01-10") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO&&&&gt;BAR\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO&&&>BAR\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities01-14") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO&#BAR\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "(1,5): expected-numeric-entity\n"
        "#new-errors\n"
        "(1:6) absence-of-digits-in-numeric-character-reference\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO&#BAR\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities01-15") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO&#ZOO\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "(1,5): expected-numeric-entity\n"
        "#new-errors\n"
        "(1:6) absence-of-digits-in-numeric-character-reference\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO&#ZOO\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities01-69") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO&#11111111111\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "(1,13): illegal-codepoint-for-numeric-entity\n"
        "(1,13): eof-in-numeric-entity\n"
        "#new-errors\n"
        "(1:17) missing-semicolon-after-character-reference\n"
        "(1:17) character-reference-outside-unicode-range\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO\357\277\275\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities01-70") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO&#1111111111\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "(1,13): illegal-codepoint-for-numeric-entity\n"
        "(1,13): eof-in-numeric-entity\n"
        "#new-errors\n"
        "(1:16) missing-semicolon-after-character-reference\n"
        "(1:16) character-reference-outside-unicode-range\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO\357\277\275\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities01-71") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO&#111111111111\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "(1,13): illegal-codepoint-for-numeric-entity\n"
        "(1,13): eof-in-numeric-entity\n"
        "#new-errors\n"
        "(1:18) missing-semicolon-after-character-reference\n"
        "(1:18) character-reference-outside-unicode-range\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO\357\277\275\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities02-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div bar=\"ZZ&gt;YY\"></div>\n"
        "#errors\n"
        "(1,20): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       bar=\"ZZ>YY\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities02-1") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div bar=\"ZZ&\"></div>\n"
        "#errors\n"
        "(1,15): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       bar=\"ZZ&\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities02-2") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div bar='ZZ&'></div>\n"
        "#errors\n"
        "(1,15): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       bar=\"ZZ&\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities02-3") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div bar=ZZ&></div>\n"
        "#errors\n"
        "(1,13): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       bar=\"ZZ&\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities02-4") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div bar=\"ZZ&gt=YY\"></div>\n"
        "#errors\n"
        "(1,20): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       bar=\"ZZ&gt=YY\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities02-5") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div bar=\"ZZ&gt0YY\"></div>\n"
        "#errors\n"
        "(1,20): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       bar=\"ZZ&gt0YY\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities02-6") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div bar=\"ZZ&gt9YY\"></div>\n"
        "#errors\n"
        "(1,20): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       bar=\"ZZ&gt9YY\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities02-7") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div bar=\"ZZ&gtaYY\"></div>\n"
        "#errors\n"
        "(1,20): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       bar=\"ZZ&gtaYY\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities02-8") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div bar=\"ZZ&gtZYY\"></div>\n"
        "#errors\n"
        "(1,20): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       bar=\"ZZ&gtZYY\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities02-9") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div bar=\"ZZ&gt YY\"></div>\n"
        "#errors\n"
        "(1,15): named-entity-without-semicolon\n"
        "(1,20): expected-doctype-but-got-start-tag\n"
        "#new-errors\n"
        "(1:16) missing-semicolon-after-character-reference\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       bar=\"ZZ> YY\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities02-10") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div bar=\"ZZ&gt\"></div>\n"
        "#errors\n"
        "(1,15): named-entity-without-semicolon\n"
        "(1,17): expected-doctype-but-got-start-tag\n"
        "#new-errors\n"
        "(1:16) missing-semicolon-after-character-reference\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       bar=\"ZZ>\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities02-11") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div bar='ZZ&gt'></div>\n"
        "#errors\n"
        "(1,15): named-entity-without-semicolon\n"
        "(1,17): expected-doctype-but-got-start-tag\n"
        "#new-errors\n"
        "(1:16) missing-semicolon-after-character-reference\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       bar=\"ZZ>\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities02-12") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div bar=ZZ&gt></div>\n"
        "#errors\n"
        "(1,14): named-entity-without-semicolon\n"
        "(1,15): expected-doctype-but-got-start-tag\n"
        "#new-errors\n"
        "(1:15) missing-semicolon-after-character-reference\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       bar=\"ZZ>\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities02-13") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div bar=\"ZZ&pound_id=23\"></div>\n"
        "#errors\n"
        "(1,18): named-entity-without-semicolon\n"
        "(1,26): expected-doctype-but-got-start-tag\n"
        "#new-errors\n"
        "(1:19) missing-semicolon-after-character-reference\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       bar=\"ZZ\302\243_id=23\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities02-14") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div bar=\"ZZ&prod_id=23\"></div>\n"
        "#errors\n"
        "(1,25): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       bar=\"ZZ&prod_id=23\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities02-15") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div bar=\"ZZ&pound;_id=23\"></div>\n"
        "#errors\n"
        "(1,27): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       bar=\"ZZ\302\243_id=23\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities02-16") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div bar=\"ZZ&prod;_id=23\"></div>\n"
        "#errors\n"
        "(1,26): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       bar=\"ZZ\342\210\217_id=23\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities02-17") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div bar=\"ZZ&pound=23\"></div>\n"
        "#errors\n"
        "(1,23): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       bar=\"ZZ&pound=23\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities02-18") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div bar=\"ZZ&prod=23\"></div>\n"
        "#errors\n"
        "(1,22): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       bar=\"ZZ&prod=23\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities02-19") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div>ZZ&pound_id=23</div>\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,13): named-entity-without-semicolon\n"
        "#new-errors\n"
        "(1:14) missing-semicolon-after-character-reference\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \"ZZ\302\243_id=23\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities02-20") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div>ZZ&prod_id=23</div>\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \"ZZ&prod_id=23\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities02-21") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div>ZZ&pound;_id=23</div>\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \"ZZ\302\243_id=23\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities02-22") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div>ZZ&prod;_id=23</div>\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \"ZZ\342\210\217_id=23\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities02-23") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div>ZZ&pound=23</div>\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,13): named-entity-without-semicolon\n"
        "#new-errors\n"
        "(1:14) missing-semicolon-after-character-reference\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \"ZZ\302\243=23\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-entities02-24") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div>ZZ&prod=23</div>\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \"ZZ&prod=23\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-html5test-com-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div<div>\n"
        "#errors\n"
        "(1,9): expected-doctype-but-got-start-tag\n"
        "(1,9): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div<div>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-html5test-com-1") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div foo<bar=''>\n"
        "#errors\n"
        "(1,9): invalid-character-in-attribute-name\n"
        "(1,16): expected-doctype-but-got-start-tag\n"
        "(1,16): expected-closing-tag-but-got-eof\n"
        "#new-errors\n"
        "(1:9) unexpected-character-in-attribute-name\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       foo<bar=\"\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-html5test-com-2") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div foo=`bar`>\n"
        "#errors\n"
        "(1,10): equals-in-unquoted-attribute-value\n"
        "(1,14): unexpected-character-in-unquoted-attribute-value\n"
        "(1,15): expected-doctype-but-got-start-tag\n"
        "(1,15): expected-closing-tag-but-got-eof\n"
        "#new-errors\n"
        "(1:10) unexpected-character-in-unquoted-attribute-value\n"
        "(1:14) unexpected-character-in-unquoted-attribute-value\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       foo=\"`bar`\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-html5test-com-3") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div \\\"foo=''>\n"
        "#errors\n"
        "(1,7): invalid-character-in-attribute-name\n"
        "(1,14): expected-doctype-but-got-start-tag\n"
        "(1,14): expected-closing-tag-but-got-eof\n"
        "#new-errors\n"
        "(1:7) unexpected-character-in-attribute-name\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \\\"foo=\"\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-html5test-com-4") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<a href='\\nbar'></a>\n"
        "#errors\n"
        "(1,16): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <a>\n"
        "|       href=\"\\nbar\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-html5test-com-5") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html>\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-html5test-com-6") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "&lang;&rang;\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"\342\237\250\342\237\251\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-html5test-com-7") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "&apos;\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"'\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-html5test-com-8") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "&ImaginaryI;\n"
        "#errors\n"
        "(1,12): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"\342\205\210\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-html5test-com-9") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "&Kopf;\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"\360\235\225\202\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-html5test-com-10") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "&notinva;\n"
        "#errors\n"
        "(1,9): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"\342\210\211\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-html5test-com-11") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<?import namespace=\"foo\" implementation=\"#bar\">\n"
        "#errors\n"
        "(1,1): expected-tag-name-but-got-question-mark\n"
        "(1,47): expected-doctype-but-got-eof\n"
        "#new-errors\n"
        "(1:2) unexpected-question-mark-instead-of-tag-name\n"
        "#document\n"
        "| <!-- ?import namespace=\"foo\" implementation=\"#bar\" -->\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-html5test-com-12") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!--foo--bar-->\n"
        "#errors\n"
        "(1,15): expected-doctype-but-got-eof\n"
        "#document\n"
        "| <!-- foo--bar -->\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-html5test-com-16") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<style><!--</style>--></style>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,30): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <style>\n"
        "|       \"<!--\"\n"
        "|   <body>\n"
        "|     \"-->\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-html5test-com-17") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<style><!--</style>-->\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <style>\n"
        "|       \"<!--\"\n"
        "|   <body>\n"
        "|     \"-->\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-html5test-com-18") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<ul><li>A </li> <li>B</li></ul>\n"
        "#errors\n"
        "(1,4): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <ul>\n"
        "|       <li>\n"
        "|         \"A \"\n"
        "|       \" \"\n"
        "|       <li>\n"
        "|         \"B\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-html5test-com-20") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<i>A<b>B<p></i>C</b>D\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,15): adoption-agency-1.3\n"
        "(1,20): adoption-agency-1.3\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <i>\n"
        "|       \"A\"\n"
        "|       <b>\n"
        "|         \"B\"\n"
        "|     <b>\n"
        "|     <p>\n"
        "|       <b>\n"
        "|         <i>\n"
        "|         \"C\"\n"
        "|       \"D\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-html5test-com-21") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div></div>\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-inbody01-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<button>1</foo>\n"
        "#errors\n"
        "(1,8): expected-doctype-but-got-start-tag\n"
        "(1,15): unexpected-end-tag\n"
        "(1,15): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <button>\n"
        "|       \"1\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-inbody01-1") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<foo>1<p>2</foo>\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,16): unexpected-end-tag\n"
        "(1,16): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <foo>\n"
        "|       \"1\"\n"
        "|       <p>\n"
        "|         \"2\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-inbody01-2") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<dd>1</foo>\n"
        "#errors\n"
        "(1,4): expected-doctype-but-got-start-tag\n"
        "(1,11): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <dd>\n"
        "|       \"1\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-isindex-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<isindex>\n"
        "#errors\n"
        "(1,9): expected-doctype-but-got-start-tag\n"
        "(1,9): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <isindex>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-isindex-1") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<isindex name=\"A\" action=\"B\" prompt=\"C\" foo=\"D\">\n"
        "#errors\n"
        "(1,48): expected-doctype-but-got-start-tag\n"
        "(1,48): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <isindex>\n"
        "|       action=\"B\"\n"
        "|       foo=\"D\"\n"
        "|       name=\"A\"\n"
        "|       prompt=\"C\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-isindex-2") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<form><isindex>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,15): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <form>\n"
        "|       <isindex>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-menuitem-element-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<menuitem>\n"
        "#errors\n"
        "10: Start tag seen without seeing a doctype first. Expected \342\200\234<!DOCTYPE html>\342\200\235.\n"
        "10: End of file seen and there were open elements.\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <menuitem>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-menuitem-element-1") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "</menuitem>\n"
        "#errors\n"
        "11: End tag seen without seeing a doctype first. Expected \342\200\234<!DOCTYPE html>\342\200\235.\n"
        "11: Stray end tag \342\200\234menuitem\342\200\235.\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-menuitem-element-2") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><menuitem>A\n"
        "#errors\n"
        "32: End of file seen and there were open elements.\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <menuitem>\n"
        "|       \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-menuitem-element-3") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><menuitem>A<menuitem>B\n"
        "#errors\n"
        "43: End of file seen and there were open elements.\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <menuitem>\n"
        "|       \"A\"\n"
        "|       <menuitem>\n"
        "|         \"B\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-menuitem-element-4") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><menuitem>A<menu>B</menu>\n"
        "#errors\n"
        "46: End of file seen and there were open elements.\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <menuitem>\n"
        "|       \"A\"\n"
        "|       <menu>\n"
        "|         \"B\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-menuitem-element-5") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><menuitem>A<hr>B\n"
        "#errors\n"
        "37: End of file seen and there were open elements.\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <menuitem>\n"
        "|       \"A\"\n"
        "|       <hr>\n"
        "|       \"B\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-menuitem-element-6") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><li><menuitem><li>\n"
        "#errors\n"
        "33: End tag \342\200\234li\342\200\235 implied, but there were open elements.\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <li>\n"
        "|       <menuitem>\n"
        "|     <li>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-menuitem-element-8") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><p><b></p><menuitem>\n"
        "#errors\n"
        "25: End tag \342\200\234p\342\200\235 seen, but there were open elements.\n"
        "35: End of file seen and there were open elements.\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <p>\n"
        "|       <b>\n"
        "|     <b>\n"
        "|       <menuitem>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-menuitem-element-9") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><menuitem><asdf></menuitem>x\n"
        "#errors\n"
        "42: End tag \342\200\234menuitem\342\200\235 seen, but there were open elements.\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <menuitem>\n"
        "|       <asdf>\n"
        "|     \"x\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-menuitem-element-10") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html></menuitem>\n"
        "#errors\n"
        "26: Stray end tag \342\200\234menuitem\342\200\235.\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-menuitem-element-11") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><html></menuitem>\n"
        "#errors\n"
        "26: Stray end tag \342\200\234menuitem\342\200\235.\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-menuitem-element-12") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><head></menuitem>\n"
        "#errors\n"
        "26: Stray end tag \342\200\234menuitem\342\200\235.\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-menuitem-element-13") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><select><menuitem></select>\n"
        "#errors\n"
        "1:34: ERROR: End tag 'select' isn't allowed here. Currently open tags: html, body, select, menuitem.\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <select>\n"
        "|       <menuitem>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-menuitem-element-14") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><option><menuitem>\n"
        "#errors\n"
        "33: End of file seen and there were open elements.\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <option>\n"
        "|       <menuitem>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-menuitem-element-15") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><menuitem><option>\n"
        "#errors\n"
        "33: End of file seen and there were open elements.\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <menuitem>\n"
        "|       <option>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-menuitem-element-16") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><menuitem></body>\n"
        "#errors\n"
        "32: End tag for  \342\200\234body\342\200\235 seen, but there were unclosed elements.\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <menuitem>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-menuitem-element-17") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><menuitem></html>\n"
        "#errors\n"
        "32: End tag for  \342\200\234html\342\200\235 seen, but there were unclosed elements.\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <menuitem>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-menuitem-element-18") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><menuitem><p>\n"
        "#errors\n"
        "28: End of file seen and there were open elements.\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <menuitem>\n"
        "|       <p>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-noscript01-2") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<head><noscript></noscript>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-tag\n"
        "#script-off\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <noscript>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-noscript01-3") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<head><noscript>   </noscript>\n"
        "#errors\n"
        "Line: 1 Col: 6 Unexpected start tag (head). Expected DOCTYPE.\n"
        "#script-off\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <noscript>\n"
        "|       \"   \"\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-noscript01-4") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<head><noscript><!--foo--></noscript>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-tag\n"
        "#script-off\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <noscript>\n"
        "|       <!-- foo -->\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-noscript01-5") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<head><noscript><basefont><!--foo--></noscript>\n"
        "#errors\n"
        "Line: 1 Col: 6 Unexpected start tag (head). Expected DOCTYPE.\n"
        "#script-off\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <noscript>\n"
        "|       <basefont>\n"
        "|       <!-- foo -->\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-noscript01-6") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<head><noscript><bgsound><!--foo--></noscript>\n"
        "#errors\n"
        "Line: 1 Col: 6 Unexpected start tag (head). Expected DOCTYPE.\n"
        "#script-off\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <noscript>\n"
        "|       <bgsound>\n"
        "|       <!-- foo -->\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-noscript01-7") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<head><noscript><link><!--foo--></noscript>\n"
        "#errors\n"
        "Line: 1 Col: 6 Unexpected start tag (head). Expected DOCTYPE.\n"
        "#script-off\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <noscript>\n"
        "|       <link>\n"
        "|       <!-- foo -->\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-noscript01-8") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<head><noscript><meta><!--foo--></noscript>\n"
        "#errors\n"
        "Line: 1 Col: 6 Unexpected start tag (head). Expected DOCTYPE.\n"
        "#script-off\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <noscript>\n"
        "|       <meta>\n"
        "|       <!-- foo -->\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-noscript01-10") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<head><noscript><style>XXX</style></noscript>\n"
        "#errors\n"
        "Line: 1 Col: 6 Unexpected start tag (head). Expected DOCTYPE.\n"
        "#script-off\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <noscript>\n"
        "|       <style>\n"
        "|         \"XXX\"\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-noscript01-11") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<head><noscript></br><!--foo--></noscript>\n"
        "#errors\n"
        "Line: 1 Col: 6 Unexpected start tag (head). Expected DOCTYPE.\n"
        "Line: 1 Col: 21 Element br not allowed in a inhead-noscript context\n"
        "Line: 1 Col: 21 Unexpected end tag (br). Treated as br element.\n"
        "Line: 1 Col: 42 Unexpected end tag (noscript). Ignored.\n"
        "#script-off\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <noscript>\n"
        "|   <body>\n"
        "|     <br>\n"
        "|     <!-- foo -->\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-noscript01-12") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<head><noscript><head class=\"foo\"><!--foo--></noscript>\n"
        "#errors\n"
        "Line: 1 Col: 6 Unexpected start tag (head). Expected DOCTYPE.\n"
        "Line: 1 Col: 34 Unexpected start tag (head).\n"
        "#script-off\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <noscript>\n"
        "|       <!-- foo -->\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-noscript01-13") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<head><noscript><noscript class=\"foo\"><!--foo--></noscript>\n"
        "#errors\n"
        "Line: 1 Col: 6 Unexpected start tag (head). Expected DOCTYPE.\n"
        "Line: 1 Col: 34 Unexpected start tag (noscript).\n"
        "#script-off\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <noscript>\n"
        "|       <!-- foo -->\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-noscript01-14") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<head><noscript></p><!--foo--></noscript>\n"
        "#errors\n"
        "Line: 1 Col: 6 Unexpected start tag (head). Expected DOCTYPE.\n"
        "Line: 1 Col: 20 Unexpected end tag (p). Ignored.\n"
        "#script-off\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <noscript>\n"
        "|       <!-- foo -->\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-noscript01-15") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<head><noscript><p><!--foo--></noscript>\n"
        "#errors\n"
        "Line: 1 Col: 6 Unexpected start tag (head). Expected DOCTYPE.\n"
        "Line: 1 Col: 19 Element p not allowed in a inhead-noscript context\n"
        "Line: 1 Col: 40 Unexpected end tag (noscript). Ignored.\n"
        "#script-off\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <noscript>\n"
        "|   <body>\n"
        "|     <p>\n"
        "|       <!-- foo -->\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-noscript01-16") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<head><noscript>XXX<!--foo--></noscript></head>\n"
        "#errors\n"
        "Line: 1 Col: 6 Unexpected start tag (head). Expected DOCTYPE.\n"
        "Line: 1 Col: 19 Unexpected non-space character. Expected inhead-noscript content\n"
        "Line: 1 Col: 30 Unexpected end tag (noscript). Ignored.\n"
        "Line: 1 Col: 37 Unexpected end tag (head). Ignored.\n"
        "#script-off\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <noscript>\n"
        "|   <body>\n"
        "|     \"XXX\"\n"
        "|     <!-- foo -->\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-pending-spec-changes-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<input type=\"hidden\"><frameset>\n"
        "#errors\n"
        "(1,21): expected-doctype-but-got-start-tag\n"
        "(1,31): unexpected-start-tag\n"
        "(1,31): eof-in-frameset\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <frameset>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-plain-text-unsafe-1") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html>\000<frameset></frameset>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,7): invalid-codepoint\n"
        "(1,7): invalid-codepoint-in-body\n"
        "(1,17): unexpected-start-tag\n"
        "#new-errors\n"
        "(1:7) unexpected-null-character\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <frameset>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-plain-text-unsafe-2") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html> \000 <frameset></frameset>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,8): invalid-codepoint\n"
        "(1,8): invalid-codepoint-in-body\n"
        "(1,19): unexpected-start-tag\n"
        "#new-errors\n"
        "(1:8) unexpected-null-character\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <frameset>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-plain-text-unsafe-3") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html>a\000a<frameset></frameset>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,8): invalid-codepoint\n"
        "(1,8): invalid-codepoint-in-body\n"
        "(1,19): unexpected-start-tag\n"
        "(1,30): unexpected-end-tag\n"
        "#new-errors\n"
        "(1:8) unexpected-null-character\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"aa\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-plain-text-unsafe-4") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html>\000\000<frameset></frameset>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,7): invalid-codepoint\n"
        "(1,7): invalid-codepoint-in-body\n"
        "(1,8): invalid-codepoint\n"
        "(1,8): invalid-codepoint-in-body\n"
        "(1,18): unexpected-start-tag\n"
        "#new-errors\n"
        "(1:7) unexpected-null-character\n"
        "(1:8) unexpected-null-character\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <frameset>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-plain-text-unsafe-5") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html>\000\n"
        "<frameset></frameset>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,7): invalid-codepoint\n"
        "(1,7): invalid-codepoint-in-body\n"
        "(2,11): unexpected-start-tag\n"
        "#new-errors\n"
        "(1:7) unexpected-null-character\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <frameset>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-plain-text-unsafe-6") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html><select>\000\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,15): invalid-codepoint\n"
        "(1,15): invalid-codepoint-in-select\n"
        "(1,15): eof-in-select\n"
        "#new-errors\n"
        "(1:15) unexpected-null-character\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <select>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-plain-text-unsafe-7") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "\000\n"
        "#errors\n"
        "(1,1): invalid-codepoint\n"
        "(1,1): expected-doctype-but-got-chars\n"
        "(1,1): invalid-codepoint-in-body\n"
        "#new-errors\n"
        "(1:1) unexpected-null-character\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-plain-text-unsafe-8") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<body>\000\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,7): invalid-codepoint\n"
        "(1,7): invalid-codepoint-in-body\n"
        "#new-errors\n"
        "(1:7) unexpected-null-character\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-plain-text-unsafe-9") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<plaintext>\000filler\000text\000\n"
        "#errors\n"
        "(1,11): expected-doctype-but-got-start-tag\n"
        "(1,12): invalid-codepoint\n"
        "(1,19): invalid-codepoint\n"
        "(1,24): invalid-codepoint\n"
        "(1,24): expected-closing-tag-but-got-eof\n"
        "#new-errors\n"
        "(1:12) unexpected-null-character\n"
        "(1:19) unexpected-null-character\n"
        "(1:24) unexpected-null-character\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <plaintext>\n"
        "|       \"\357\277\275filler\357\277\275text\357\277\275\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-plain-text-unsafe-11") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<body><!\000>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,8): expected-dashes-or-doctype\n"
        "(1,9): unexpected-null-character\n"
        "#new-errors\n"
        "(1:9) incorrectly-opened-comment\n"
        "(1:9) unexpected-null-character\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <!-- \357\277\275 -->\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-plain-text-unsafe-12") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<body><!\000filler\000text>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,8): expected-dashes-or-doctype\n"
        "(1:9) unexpected-null-character\n"
        "(1:16) unexpected-null-character\n"
        "#new-errors\n"
        "(1:9) incorrectly-opened-comment\n"
        "(1:9) unexpected-null-character\n"
        "(1:16) unexpected-null-character\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <!-- \357\277\275filler\357\277\275text -->\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-plain-text-unsafe-18") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<svg>\000</svg><frameset>\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,6): invalid-codepoint\n"
        "(1,6): invalid-codepoint-in-foreign-content\n"
        "(1,22): unexpected-start-tag\n"
        "(1,22): eof-in-frameset\n"
        "#new-errors\n"
        "(1:6) unexpected-null-character\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <frameset>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-plain-text-unsafe-19") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<svg>\000 </svg><frameset>\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,6): invalid-codepoint\n"
        "(1,6): invalid-codepoint-in-foreign-content\n"
        "(1,23): unexpected-start-tag\n"
        "(1,23): eof-in-frameset\n"
        "#new-errors\n"
        "(1:6) unexpected-null-character\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <frameset>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-plain-text-unsafe-21") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<svg><path></path></svg><frameset>\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,34): unexpected-start-tag\n"
        "(1,34): eof-in-frameset\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <frameset>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-ruby-4") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html><ruby>a<rb>b<span></ruby></html>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,31): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <ruby>\n"
        "|       \"a\"\n"
        "|       <rb>\n"
        "|         \"b\"\n"
        "|         <span>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-ruby-9") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html><ruby>a<rt>b<span></ruby></html>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,31): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <ruby>\n"
        "|       \"a\"\n"
        "|       <rt>\n"
        "|         \"b\"\n"
        "|         <span>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-ruby-13") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html><ruby>a<rtc>b<rp></ruby></html>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <ruby>\n"
        "|       \"a\"\n"
        "|       <rtc>\n"
        "|         \"b\"\n"
        "|         <rp>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-ruby-14") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html><ruby>a<rtc>b<span></ruby></html>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,32): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <ruby>\n"
        "|       \"a\"\n"
        "|       <rtc>\n"
        "|         \"b\"\n"
        "|         <span>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-ruby-19") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html><ruby>a<rp>b<span></ruby></html>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,31): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <ruby>\n"
        "|       \"a\"\n"
        "|       <rp>\n"
        "|         \"b\"\n"
        "|         <span>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-scriptdata01-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO<script>'Hello'</script>BAR\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO\"\n"
        "|     <script>\n"
        "|       \"'Hello'\"\n"
        "|     \"BAR\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-scriptdata01-5") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO<script type=\"text/plain\"></scriptx>BAR\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "(1,42): expected-named-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO\"\n"
        "|     <script>\n"
        "|       type=\"text/plain\"\n"
        "|       \"</scriptx>BAR\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-scriptdata01-7") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO<script>'<'</script>BAR\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO\"\n"
        "|     <script>\n"
        "|       \"'<'\"\n"
        "|     \"BAR\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-scriptdata01-8") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO<script>'<!'</script>BAR\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO\"\n"
        "|     <script>\n"
        "|       \"'<!'\"\n"
        "|     \"BAR\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-scriptdata01-9") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO<script>'<!-'</script>BAR\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO\"\n"
        "|     <script>\n"
        "|       \"'<!-'\"\n"
        "|     \"BAR\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-scriptdata01-10") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO<script>'<!--'</script>BAR\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO\"\n"
        "|     <script>\n"
        "|       \"'<!--'\"\n"
        "|     \"BAR\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-scriptdata01-11") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO<script>'<!---'</script>BAR\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO\"\n"
        "|     <script>\n"
        "|       \"'<!---'\"\n"
        "|     \"BAR\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-scriptdata01-12") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO<script>'<!-->'</script>BAR\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO\"\n"
        "|     <script>\n"
        "|       \"'<!-->'\"\n"
        "|     \"BAR\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-scriptdata01-13") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO<script>'<!-- potato'</script>BAR\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO\"\n"
        "|     <script>\n"
        "|       \"'<!-- potato'\"\n"
        "|     \"BAR\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-scriptdata01-14") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO<script>'<!-- <sCrIpt'</script>BAR\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO\"\n"
        "|     <script>\n"
        "|       \"'<!-- <sCrIpt'\"\n"
        "|     \"BAR\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-scriptdata01-18") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO<script>'<!-- <sCrIpt> -->'</script>BAR\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO\"\n"
        "|     <script>\n"
        "|       \"'<!-- <sCrIpt> -->'\"\n"
        "|     \"BAR\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-scriptdata01-23") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "FOO<script type=\"text/plain\">'<!-- <sCrIpt\\'</script>BAR\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"FOO\"\n"
        "|     <script>\n"
        "|       type=\"text/plain\"\n"
        "|       \"'<!-- <sCrIpt\\'\"\n"
        "|     \"BAR\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tables01-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><th>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,11): unexpected-cell-in-table-body\n"
        "(1,11): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|           <th>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tables01-1") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><td>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,11): unexpected-cell-in-table-body\n"
        "(1,11): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|           <td>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tables01-2") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><col foo='bar'>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,22): eof-in-table\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <colgroup>\n"
        "|         <col>\n"
        "|           foo=\"bar\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tables01-3") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><colgroup></html>foo\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,24): unexpected-end-tag\n"
        "(1,27): foster-parenting-character-in-table\n"
        "(1,27): foster-parenting-character-in-table\n"
        "(1,27): foster-parenting-character-in-table\n"
        "(1,27): eof-in-table\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"foo\"\n"
        "|     <table>\n"
        "|       <colgroup>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tables01-4") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table></table><p>foo\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|     <p>\n"
        "|       \"foo\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tables01-5") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table></body></caption></col></colgroup></html></tbody></td></tfoot></th></thead></tr><td>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,14): unexpected-end-tag\n"
        "(1,24): unexpected-end-tag\n"
        "(1,30): unexpected-end-tag\n"
        "(1,41): unexpected-end-tag\n"
        "(1,48): unexpected-end-tag\n"
        "(1,56): unexpected-end-tag\n"
        "(1,61): unexpected-end-tag\n"
        "(1,69): unexpected-end-tag\n"
        "(1,74): unexpected-end-tag\n"
        "(1,82): unexpected-end-tag\n"
        "(1,87): unexpected-end-tag\n"
        "(1,91): unexpected-cell-in-table-body\n"
        "(1,91): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|           <td>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tables01-6") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><select><option>3</select></table>\n"
        "#errors\n"
        "1:1: ERROR: Expected a doctype token\n"
        "1:8: ERROR: Start tag 'select' isn't allowed here. Currently open tags: html, body, table.\n"
        "1:16: ERROR: Start tag 'option' isn't allowed here. Currently open tags: html, body, table, select.\n"
        "1:24: ERROR: Character tokens aren't legal here\n"
        "1:25: ERROR: End tag 'select' isn't allowed here. Currently open tags: html, body, table, select, option.\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <select>\n"
        "|       <option>\n"
        "|         \"3\"\n"
        "|     <table>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tables01-7") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><select><table></table></select></table>\n"
        "#errors\n"
        "1:1: ERROR: Expected a doctype token\n"
        "1:8: ERROR: Start tag 'select' isn't allowed here. Currently open tags: html, body, table.\n"
        "1:16: ERROR: Start tag 'table' isn't allowed here. Currently open tags: html, body, table, select.\n"
        "1:31: ERROR: End tag 'select' isn't allowed here. Currently open tags: html, body.\n"
        "1:40: ERROR: End tag 'table' isn't allowed here. Currently open tags: html, body.\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <select>\n"
        "|     <table>\n"
        "|     <table>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tables01-8") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><select></table>\n"
        "#errors\n"
        "1:1: ERROR: Expected a doctype token\n"
        "1:8: ERROR: Start tag 'select' isn't allowed here. Currently open tags: html, body, table.\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <select>\n"
        "|     <table>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tables01-9") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><select><option>A<tr><td>B</td></tr></table>\n"
        "#errors\n"
        "1:1: ERROR: Expected a doctype token\n"
        "1:8: ERROR: Start tag 'select' isn't allowed here. Currently open tags: html, body, table.\n"
        "1:16: ERROR: Start tag 'option' isn't allowed here. Currently open tags: html, body, table, select.\n"
        "1:24: ERROR: Character tokens aren't legal here\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <select>\n"
        "|       <option>\n"
        "|         \"A\"\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|           <td>\n"
        "|             \"B\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tables01-10") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><td></body></caption></col></colgroup></html>foo\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,11): unexpected-cell-in-table-body\n"
        "(1,18): unexpected-end-tag\n"
        "(1,28): unexpected-end-tag\n"
        "(1,34): unexpected-end-tag\n"
        "(1,45): unexpected-end-tag\n"
        "(1,52): unexpected-end-tag\n"
        "(1,55): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|           <td>\n"
        "|             \"foo\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tables01-11") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><td>A</table>B\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,11): unexpected-cell-in-table-body\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|           <td>\n"
        "|             \"A\"\n"
        "|     \"B\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tables01-12") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><tr><caption>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,20): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|       <caption>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tables01-13") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><tr></body></caption></col></colgroup></html></td></th><td>foo\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,18): unexpected-end-tag-in-table-row\n"
        "(1,28): unexpected-end-tag-in-table-row\n"
        "(1,34): unexpected-end-tag-in-table-row\n"
        "(1,45): unexpected-end-tag-in-table-row\n"
        "(1,52): unexpected-end-tag-in-table-row\n"
        "(1,57): unexpected-end-tag-in-table-row\n"
        "(1,62): unexpected-end-tag-in-table-row\n"
        "(1,69): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|           <td>\n"
        "|             \"foo\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tables01-14") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><td><tr>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,11): unexpected-cell-in-table-body\n"
        "(1,15): eof-in-table\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|           <td>\n"
        "|         <tr>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tables01-15") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><td><button><td>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,11): unexpected-cell-in-table-body\n"
        "(1,23): unexpected-cell-end-tag\n"
        "(1,23): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|           <td>\n"
        "|             <button>\n"
        "|           <td>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-template-7") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div></template></div>\n"
        "#errors\n"
        " * (1,6) missing DOCTYPE\n"
        " * (1,17) unexpected template end tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "Test\n"
        "#errors\n"
        "(1,0): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"Test\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-1") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<p>One<p>Two\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <p>\n"
        "|       \"One\"\n"
        "|     <p>\n"
        "|       \"Two\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-2") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "Line1<br>Line2<br>Line3<br>Line4\n"
        "#errors\n"
        "(1,0): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"Line1\"\n"
        "|     <br>\n"
        "|     \"Line2\"\n"
        "|     <br>\n"
        "|     \"Line3\"\n"
        "|     <br>\n"
        "|     \"Line4\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-3") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-4") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<head>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-5") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<body>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-6") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html><head>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-7") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html><head></head>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-8") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html><head></head><body>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-9") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html><head></head><body></body>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-10") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html><head><body></body></html>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-11") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html><head></body></html>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-12") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html><head><body></html>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-13") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html><body></html>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-14") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<body></html>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-15") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<head></html>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-16") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "</head>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-17") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "</body>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-end-tag element.\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-18") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "</html>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-end-tag element.\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-19") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<b><table><td><i></table>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,14): unexpected-cell-in-table-body\n"
        "(1,25): unexpected-cell-end-tag\n"
        "(1,25): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <b>\n"
        "|       <table>\n"
        "|         <tbody>\n"
        "|           <tr>\n"
        "|             <td>\n"
        "|               <i>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-21") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<h1>Hello<h2>World\n"
        "#errors\n"
        "(1,4): expected-doctype-but-got-start-tag\n"
        "(1,13): unexpected-start-tag\n"
        "(1,18): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <h1>\n"
        "|       \"Hello\"\n"
        "|     <h2>\n"
        "|       \"World\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-22") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<a><p>X<a>Y</a>Z</p></a>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,10): unexpected-start-tag-implies-end-tag\n"
        "(1,10): adoption-agency-1.3\n"
        "(1,24): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <a>\n"
        "|     <p>\n"
        "|       <a>\n"
        "|         \"X\"\n"
        "|       <a>\n"
        "|         \"Y\"\n"
        "|       \"Z\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-23") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<b><button>foo</b>bar\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,18): adoption-agency-1.3\n"
        "(1,21): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <b>\n"
        "|     <button>\n"
        "|       <b>\n"
        "|         \"foo\"\n"
        "|       \"bar\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-25") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<p><b><div><marquee></p></b></div>X\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,11): unexpected-end-tag\n"
        "(1,24): unexpected-end-tag\n"
        "(1,28): unexpected-end-tag\n"
        "(1,34): end-tag-too-early\n"
        "(1,35): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <p>\n"
        "|       <b>\n"
        "|     <div>\n"
        "|       <b>\n"
        "|         <marquee>\n"
        "|           <p>\n"
        "|           \"X\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-26") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<script><div></script></div><title><p></title><p><p>\n"
        "#errors\n"
        "(1,8): expected-doctype-but-got-start-tag\n"
        "(1,28): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <script>\n"
        "|       \"<div>\"\n"
        "|     <title>\n"
        "|       \"<p>\"\n"
        "|   <body>\n"
        "|     <p>\n"
        "|     <p>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-27") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!--><div>--<!-->\n"
        "#errors\n"
        "(1,5): incorrect-comment\n"
        "(1,10): expected-doctype-but-got-start-tag\n"
        "(1,17): incorrect-comment\n"
        "(1,17): expected-closing-tag-but-got-eof\n"
        "#new-errors\n"
        "(1:5) abrupt-closing-of-empty-comment\n"
        "(1:17) abrupt-closing-of-empty-comment\n"
        "#document\n"
        "| <!--  -->\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \"--\"\n"
        "|       <!--  -->\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-28") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<p><hr></p>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,11): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <p>\n"
        "|     <hr>\n"
        "|     <p>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-30") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<a><table><td><a><table></table><a></tr><a></table><b>X</b>C<a>Y\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,14): unexpected-cell-in-table-body\n"
        "(1,35): unexpected-start-tag-implies-end-tag\n"
        "(1,40): unexpected-cell-end-tag\n"
        "(1,43): unexpected-start-tag-implies-table-voodoo\n"
        "(1,43): unexpected-start-tag-implies-end-tag\n"
        "(1,43): unexpected-end-tag\n"
        "(1,63): unexpected-start-tag-implies-end-tag\n"
        "(1,64): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <a>\n"
        "|       <a>\n"
        "|       <table>\n"
        "|         <tbody>\n"
        "|           <tr>\n"
        "|             <td>\n"
        "|               <a>\n"
        "|                 <table>\n"
        "|               <a>\n"
        "|     <a>\n"
        "|       <b>\n"
        "|         \"X\"\n"
        "|       \"C\"\n"
        "|     <a>\n"
        "|       \"Y\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-31") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<a X>0<b>1<a Y>2\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,15): unexpected-start-tag-implies-end-tag\n"
        "(1,15): adoption-agency-1.3\n"
        "(1,16): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <a>\n"
        "|       x=\"\"\n"
        "|       \"0\"\n"
        "|       <b>\n"
        "|         \"1\"\n"
        "|     <b>\n"
        "|       <a>\n"
        "|         y=\"\"\n"
        "|         \"2\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-35") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<\n"
        "#errors\n"
        "(1,1): expected-tag-name\n"
        "(1,1): expected-doctype-but-got-chars\n"
        "#new-errors\n"
        "(1:2) eof-before-tag-name\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"<\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-36") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<#\n"
        "#errors\n"
        "(1,1): expected-tag-name\n"
        "(1,1): expected-doctype-but-got-chars\n"
        "#new-errors\n"
        "(1:2) invalid-first-character-of-tag-name\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"<#\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-37") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "</\n"
        "#errors\n"
        "(1,2): expected-closing-tag-but-got-eof\n"
        "(1,2): expected-doctype-but-got-chars\n"
        "#new-errors\n"
        "(1:3) eof-before-tag-name\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"</\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-41") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!\n"
        "#errors\n"
        "(1,2): expected-dashes-or-doctype\n"
        "(1,2): expected-doctype-but-got-eof\n"
        "#new-errors\n"
        "(1:3) incorrectly-opened-comment\n"
        "#document\n"
        "| <!--  -->\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-43") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<?COMMENT?>\n"
        "#errors\n"
        "(1,1): expected-tag-name-but-got-question-mark\n"
        "(1,11): expected-doctype-but-got-eof\n"
        "#new-errors\n"
        "(1:2) unexpected-question-mark-instead-of-tag-name\n"
        "#document\n"
        "| <!-- ?COMMENT? -->\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-44") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!COMMENT>\n"
        "#errors\n"
        "(1,2): expected-dashes-or-doctype\n"
        "(1,10): expected-doctype-but-got-eof\n"
        "#new-errors\n"
        "(1:3) incorrectly-opened-comment\n"
        "#document\n"
        "| <!-- COMMENT -->\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-45") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "</ COMMENT >\n"
        "#errors\n"
        "(1,2): expected-closing-tag-but-got-char\n"
        "(1,12): expected-doctype-but-got-eof\n"
        "#new-errors\n"
        "(1:3) invalid-first-character-of-tag-name\n"
        "#document\n"
        "| <!--  COMMENT  -->\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-46") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<?COM--MENT?>\n"
        "#errors\n"
        "(1,1): expected-tag-name-but-got-question-mark\n"
        "(1,13): expected-doctype-but-got-eof\n"
        "#new-errors\n"
        "(1:2) unexpected-question-mark-instead-of-tag-name\n"
        "#document\n"
        "| <!-- ?COM--MENT? -->\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-47") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!COM--MENT>\n"
        "#errors\n"
        "(1,2): expected-dashes-or-doctype\n"
        "(1,12): expected-doctype-but-got-eof\n"
        "#new-errors\n"
        "(1:3) incorrectly-opened-comment\n"
        "#document\n"
        "| <!-- COM--MENT -->\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-48") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "</ COM--MENT >\n"
        "#errors\n"
        "(1,2): expected-closing-tag-but-got-char\n"
        "(1,14): expected-doctype-but-got-eof\n"
        "#new-errors\n"
        "(1:3) invalid-first-character-of-tag-name\n"
        "#document\n"
        "| <!--  COM--MENT  -->\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-50") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><script> <!-- </script> --> </script> EOF\n"
        "#errors\n"
        "(1,52): unexpected-end-tag\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <script>\n"
        "|       \" <!-- \"\n"
        "|     \" \"\n"
        "|   <body>\n"
        "|     \"-->  EOF\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-51") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<b><p></b>TEST\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,10): adoption-agency-1.3\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <b>\n"
        "|     <p>\n"
        "|       <b>\n"
        "|       \"TEST\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-52") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<p id=a><b><p id=b></b>TEST\n"
        "#errors\n"
        "(1,8): expected-doctype-but-got-start-tag\n"
        "(1,19): unexpected-end-tag\n"
        "(1,23): adoption-agency-1.2\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <p>\n"
        "|       id=\"a\"\n"
        "|       <b>\n"
        "|     <p>\n"
        "|       id=\"b\"\n"
        "|       \"TEST\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-54") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><title>U-test</title><body><div><p>Test<u></p></div></body>\n"
        "#errors\n"
        "(1,61): unexpected-end-tag\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <title>\n"
        "|       \"U-test\"\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       <p>\n"
        "|         \"Test\"\n"
        "|         <u>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-55") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><font><table></font></table></font>\n"
        "#errors\n"
        "(1,35): unexpected-end-tag-implies-table-voodoo\n"
        "(1,35): unexpected-end-tag\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <font>\n"
        "|       <table>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-56") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<font><p>hello<b>cruel</font>world\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,29): adoption-agency-1.3\n"
        "(1,29): adoption-agency-1.3\n"
        "(1,34): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <font>\n"
        "|     <p>\n"
        "|       <font>\n"
        "|         \"hello\"\n"
        "|         <b>\n"
        "|           \"cruel\"\n"
        "|       <b>\n"
        "|         \"world\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-57") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<b>Test</i>Test\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,11): unexpected-end-tag\n"
        "(1,15): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <b>\n"
        "|       \"TestTest\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-58") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<b>A<cite>B<div>C\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,17): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <b>\n"
        "|       \"A\"\n"
        "|       <cite>\n"
        "|         \"B\"\n"
        "|         <div>\n"
        "|           \"C\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-60") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<b>A<cite>B<div>C</b>D\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,21): adoption-agency-1.3\n"
        "(1,22): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <b>\n"
        "|       \"A\"\n"
        "|       <cite>\n"
        "|         \"B\"\n"
        "|     <div>\n"
        "|       <b>\n"
        "|         \"C\"\n"
        "|       \"D\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-63") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<DIV>\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,5): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-64") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<DIV> abc\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,9): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \" abc\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-65") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<DIV> abc <B>\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,13): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \" abc \"\n"
        "|       <b>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-66") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<DIV> abc <B> def\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,17): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \" abc \"\n"
        "|       <b>\n"
        "|         \" def\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-67") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<DIV> abc <B> def <I>\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,21): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \" abc \"\n"
        "|       <b>\n"
        "|         \" def \"\n"
        "|         <i>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-68") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<DIV> abc <B> def <I> ghi\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,25): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \" abc \"\n"
        "|       <b>\n"
        "|         \" def \"\n"
        "|         <i>\n"
        "|           \" ghi\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-69") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<DIV> abc <B> def <I> ghi <P>\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,29): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \" abc \"\n"
        "|       <b>\n"
        "|         \" def \"\n"
        "|         <i>\n"
        "|           \" ghi \"\n"
        "|           <p>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-70") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<DIV> abc <B> def <I> ghi <P> jkl\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,33): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \" abc \"\n"
        "|       <b>\n"
        "|         \" def \"\n"
        "|         <i>\n"
        "|           \" ghi \"\n"
        "|           <p>\n"
        "|             \" jkl\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-71") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<DIV> abc <B> def <I> ghi <P> jkl </B>\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,38): adoption-agency-1.3\n"
        "(1,38): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \" abc \"\n"
        "|       <b>\n"
        "|         \" def \"\n"
        "|         <i>\n"
        "|           \" ghi \"\n"
        "|       <i>\n"
        "|         <p>\n"
        "|           <b>\n"
        "|             \" jkl \"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-72") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<DIV> abc <B> def <I> ghi <P> jkl </B> mno\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,38): adoption-agency-1.3\n"
        "(1,42): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \" abc \"\n"
        "|       <b>\n"
        "|         \" def \"\n"
        "|         <i>\n"
        "|           \" ghi \"\n"
        "|       <i>\n"
        "|         <p>\n"
        "|           <b>\n"
        "|             \" jkl \"\n"
        "|           \" mno\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-73") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<DIV> abc <B> def <I> ghi <P> jkl </B> mno </I>\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,38): adoption-agency-1.3\n"
        "(1,47): adoption-agency-1.3\n"
        "(1,47): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \" abc \"\n"
        "|       <b>\n"
        "|         \" def \"\n"
        "|         <i>\n"
        "|           \" ghi \"\n"
        "|       <i>\n"
        "|       <p>\n"
        "|         <i>\n"
        "|           <b>\n"
        "|             \" jkl \"\n"
        "|           \" mno \"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-74") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<DIV> abc <B> def <I> ghi <P> jkl </B> mno </I> pqr\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,38): adoption-agency-1.3\n"
        "(1,47): adoption-agency-1.3\n"
        "(1,51): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \" abc \"\n"
        "|       <b>\n"
        "|         \" def \"\n"
        "|         <i>\n"
        "|           \" ghi \"\n"
        "|       <i>\n"
        "|       <p>\n"
        "|         <i>\n"
        "|           <b>\n"
        "|             \" jkl \"\n"
        "|           \" mno \"\n"
        "|         \" pqr\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-75") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<DIV> abc <B> def <I> ghi <P> jkl </B> mno </I> pqr </P>\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,38): adoption-agency-1.3\n"
        "(1,47): adoption-agency-1.3\n"
        "(1,56): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \" abc \"\n"
        "|       <b>\n"
        "|         \" def \"\n"
        "|         <i>\n"
        "|           \" ghi \"\n"
        "|       <i>\n"
        "|       <p>\n"
        "|         <i>\n"
        "|           <b>\n"
        "|             \" jkl \"\n"
        "|           \" mno \"\n"
        "|         \" pqr \"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-76") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<DIV> abc <B> def <I> ghi <P> jkl </B> mno </I> pqr </P> stu\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,38): adoption-agency-1.3\n"
        "(1,47): adoption-agency-1.3\n"
        "(1,60): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \" abc \"\n"
        "|       <b>\n"
        "|         \" def \"\n"
        "|         <i>\n"
        "|           \" ghi \"\n"
        "|       <i>\n"
        "|       <p>\n"
        "|         <i>\n"
        "|           <b>\n"
        "|             \" jkl \"\n"
        "|           \" mno \"\n"
        "|         \" pqr \"\n"
        "|       \" stu\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-77") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<test attribute---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------->\n"
        "#errors\n"
        "(1,1040): expected-doctype-but-got-start-tag\n"
        "(1,1040): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <test>\n"
        "|       attribute----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------=\"\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-78") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<a href=\"blah\">aba<table><a href=\"foo\">br<tr><td></td></tr>x</table>aoe\n"
        "#errors\n"
        "(1,15): expected-doctype-but-got-start-tag\n"
        "(1,39): unexpected-start-tag-implies-table-voodoo\n"
        "(1,39): unexpected-start-tag-implies-end-tag\n"
        "(1,39): unexpected-end-tag\n"
        "(1,45): foster-parenting-character-in-table\n"
        "(1,45): foster-parenting-character-in-table\n"
        "(1,68): foster-parenting-character-in-table\n"
        "(1,71): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <a>\n"
        "|       href=\"blah\"\n"
        "|       \"aba\"\n"
        "|       <a>\n"
        "|         href=\"foo\"\n"
        "|         \"br\"\n"
        "|       <a>\n"
        "|         href=\"foo\"\n"
        "|         \"x\"\n"
        "|       <table>\n"
        "|         <tbody>\n"
        "|           <tr>\n"
        "|             <td>\n"
        "|     <a>\n"
        "|       href=\"foo\"\n"
        "|       \"aoe\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-81") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<a href=a>aa<marquee>aa<a href=b>bb</marquee>aa\n"
        "#errors\n"
        "(1,10): expected-doctype-but-got-start-tag\n"
        "(1,45): end-tag-too-early\n"
        "(1,47): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <a>\n"
        "|       href=\"a\"\n"
        "|       \"aa\"\n"
        "|       <marquee>\n"
        "|         \"aa\"\n"
        "|         <a>\n"
        "|           href=\"b\"\n"
        "|           \"bb\"\n"
        "|       \"aa\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-82") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<wbr><strike><code></strike><code><strike></code>\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,28): adoption-agency-1.3\n"
        "(1,49): adoption-agency-1.3\n"
        "(1,49): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <wbr>\n"
        "|     <strike>\n"
        "|       <code>\n"
        "|     <code>\n"
        "|       <code>\n"
        "|         <strike>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-83") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><spacer>foo\n"
        "#errors\n"
        "(1,26): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <spacer>\n"
        "|       \"foo\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-84") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<title><meta></title><link><title><meta></title>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <title>\n"
        "|       \"<meta>\"\n"
        "|     <link>\n"
        "|     <title>\n"
        "|       \"<meta>\"\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-85") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<style><!--</style><meta><script>--><link></script>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <style>\n"
        "|       \"<!--\"\n"
        "|     <meta>\n"
        "|     <script>\n"
        "|       \"--><link>\"\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-86") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<head><meta></head><link>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,25): unexpected-start-tag-out-of-my-head\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <meta>\n"
        "|     <link>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-87") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><tr><tr><td><td><span><th><span>X</table>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,33): unexpected-cell-end-tag\n"
        "(1,48): unexpected-cell-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|         <tr>\n"
        "|           <td>\n"
        "|           <td>\n"
        "|             <span>\n"
        "|           <th>\n"
        "|             <span>\n"
        "|               \"X\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-88") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<body><body><base><link><meta><title><p></title><body><p></body>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,12): unexpected-start-tag\n"
        "(1,54): unexpected-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <base>\n"
        "|     <link>\n"
        "|     <meta>\n"
        "|     <title>\n"
        "|       \"<p>\"\n"
        "|     <p>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-91") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<a><table><a></table><p><a><div><a>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,13): unexpected-start-tag-implies-table-voodoo\n"
        "(1,13): unexpected-start-tag-implies-end-tag\n"
        "(1,13): adoption-agency-1.3\n"
        "(1,27): unexpected-start-tag-implies-end-tag\n"
        "(1,27): adoption-agency-1.2\n"
        "(1,32): unexpected-end-tag\n"
        "(1,35): unexpected-start-tag-implies-end-tag\n"
        "(1,35): adoption-agency-1.2\n"
        "(1,35): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <a>\n"
        "|       <a>\n"
        "|       <table>\n"
        "|     <p>\n"
        "|       <a>\n"
        "|     <div>\n"
        "|       <a>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-92") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<head></p><meta><p>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,10): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <meta>\n"
        "|   <body>\n"
        "|     <p>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-94") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<b><table><td></b><i></table>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,14): unexpected-cell-in-table-body\n"
        "(1,18): unexpected-end-tag\n"
        "(1,29): unexpected-cell-end-tag\n"
        "(1,29): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <b>\n"
        "|       <table>\n"
        "|         <tbody>\n"
        "|           <tr>\n"
        "|             <td>\n"
        "|               <i>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-95") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<h1><h2>\n"
        "#errors\n"
        "(1,4): expected-doctype-but-got-start-tag\n"
        "(1,8): unexpected-start-tag\n"
        "(1,8): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <h1>\n"
        "|     <h2>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-96") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<a><p><a></a></p></a>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,9): unexpected-start-tag-implies-end-tag\n"
        "(1,9): adoption-agency-1.3\n"
        "(1,21): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <a>\n"
        "|     <p>\n"
        "|       <a>\n"
        "|       <a>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-97") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<b><button></b></button></b>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,15): adoption-agency-1.3\n"
        "(1,28): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <b>\n"
        "|     <button>\n"
        "|       <b>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-98") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<p><b><div><marquee></p></b></div>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,11): unexpected-end-tag\n"
        "(1,24): unexpected-end-tag\n"
        "(1,28): unexpected-end-tag\n"
        "(1,34): end-tag-too-early\n"
        "(1,34): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <p>\n"
        "|       <b>\n"
        "|     <div>\n"
        "|       <b>\n"
        "|         <marquee>\n"
        "|           <p>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-102") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<a><table><td><a><table></table><a></tr><a></table><a>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,14): unexpected-cell-in-table-body\n"
        "(1,35): unexpected-start-tag-implies-end-tag\n"
        "(1,40): unexpected-cell-end-tag\n"
        "(1,43): unexpected-start-tag-implies-table-voodoo\n"
        "(1,43): unexpected-start-tag-implies-end-tag\n"
        "(1,43): unexpected-end-tag\n"
        "(1,54): unexpected-start-tag-implies-end-tag\n"
        "(1,54): adoption-agency-1.2\n"
        "(1,54): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <a>\n"
        "|       <a>\n"
        "|       <table>\n"
        "|         <tbody>\n"
        "|           <tr>\n"
        "|             <td>\n"
        "|               <a>\n"
        "|                 <table>\n"
        "|               <a>\n"
        "|     <a>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-103") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<ul><li></li><div><li></div><li><li><div><li><address><li><b><em></b><li></ul>\n"
        "#errors\n"
        "(1,4): expected-doctype-but-got-start-tag\n"
        "(1,45): end-tag-too-early\n"
        "(1,58): end-tag-too-early\n"
        "(1,69): adoption-agency-1.3\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <ul>\n"
        "|       <li>\n"
        "|       <div>\n"
        "|         <li>\n"
        "|       <li>\n"
        "|       <li>\n"
        "|         <div>\n"
        "|       <li>\n"
        "|         <address>\n"
        "|       <li>\n"
        "|         <b>\n"
        "|           <em>\n"
        "|       <li>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-104") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<ul><li><ul></li><li>a</li></ul></li></ul>\n"
        "#errors\n"
        "(1,4): expected-doctype-but-got-start-tag\n"
        "(1,17): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <ul>\n"
        "|       <li>\n"
        "|         <ul>\n"
        "|           <li>\n"
        "|             \"a\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-106") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<h1><table><td><h3></table><h3></h1>\n"
        "#errors\n"
        "(1,4): expected-doctype-but-got-start-tag\n"
        "(1,15): unexpected-cell-in-table-body\n"
        "(1,27): unexpected-cell-end-tag\n"
        "(1,31): unexpected-start-tag\n"
        "(1,36): end-tag-too-early\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <h1>\n"
        "|       <table>\n"
        "|         <tbody>\n"
        "|           <tr>\n"
        "|             <td>\n"
        "|               <h3>\n"
        "|     <h3>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-107") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><colgroup><col><colgroup><col><col><col><colgroup><col><col><thead><tr><td></table>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <colgroup>\n"
        "|         <col>\n"
        "|       <colgroup>\n"
        "|         <col>\n"
        "|         <col>\n"
        "|         <col>\n"
        "|       <colgroup>\n"
        "|         <col>\n"
        "|         <col>\n"
        "|       <thead>\n"
        "|         <tr>\n"
        "|           <td>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-108") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><col><tbody><col><tr><col><td><col></table><col>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,37): unexpected-cell-in-table-body\n"
        "(1,55): unexpected-start-tag-ignored\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <colgroup>\n"
        "|         <col>\n"
        "|       <tbody>\n"
        "|       <colgroup>\n"
        "|         <col>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|       <colgroup>\n"
        "|         <col>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|           <td>\n"
        "|       <colgroup>\n"
        "|         <col>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-109") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><colgroup><tbody><colgroup><tr><colgroup><td><colgroup></table><colgroup>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,52): unexpected-cell-in-table-body\n"
        "(1,80): unexpected-start-tag-ignored\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <colgroup>\n"
        "|       <tbody>\n"
        "|       <colgroup>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|       <colgroup>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|           <td>\n"
        "|       <colgroup>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests1-111") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><tr></strong></b></em></i></u></strike></s></blink></tt></pre></big></small></font></select></h1></h2></h3></h4></h5></h6></body></br></a></img></title></span></style></script></table></th></td></tr></frame></area></link></param></hr></input></col></base></meta></basefont></bgsound></embed></spacer></p></dd></dt></caption></colgroup></tbody></tfoot></thead></address></blockquote></center></dir></div></dl></fieldset></listing></menu></ol></ul></li></nobr></wbr></form></button></marquee></object></html></frameset></head></iframe></image></isindex></noembed></noframes></noscript></optgroup></option></plaintext></textarea>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,20): unexpected-end-tag-implies-table-voodoo\n"
        "(1,20): unexpected-end-tag\n"
        "(1,24): unexpected-end-tag-implies-table-voodoo\n"
        "(1,24): unexpected-end-tag\n"
        "(1,29): unexpected-end-tag-implies-table-voodoo\n"
        "(1,29): unexpected-end-tag\n"
        "(1,33): unexpected-end-tag-implies-table-voodoo\n"
        "(1,33): unexpected-end-tag\n"
        "(1,37): unexpected-end-tag-implies-table-voodoo\n"
        "(1,37): unexpected-end-tag\n"
        "(1,46): unexpected-end-tag-implies-table-voodoo\n"
        "(1,46): unexpected-end-tag\n"
        "(1,50): unexpected-end-tag-implies-table-voodoo\n"
        "(1,50): unexpected-end-tag\n"
        "(1,58): unexpected-end-tag-implies-table-voodoo\n"
        "(1,58): unexpected-end-tag\n"
        "(1,63): unexpected-end-tag-implies-table-voodoo\n"
        "(1,63): unexpected-end-tag\n"
        "(1,69): unexpected-end-tag-implies-table-voodoo\n"
        "(1,69): end-tag-too-early\n"
        "(1,75): unexpected-end-tag-implies-table-voodoo\n"
        "(1,75): unexpected-end-tag\n"
        "(1,83): unexpected-end-tag-implies-table-voodoo\n"
        "(1,83): unexpected-end-tag\n"
        "(1,90): unexpected-end-tag-implies-table-voodoo\n"
        "(1,90): unexpected-end-tag\n"
        "(1,99): unexpected-end-tag-implies-table-voodoo\n"
        "(1,99): unexpected-end-tag\n"
        "(1,104): unexpected-end-tag-implies-table-voodoo\n"
        "(1,104): end-tag-too-early\n"
        "(1,109): unexpected-end-tag-implies-table-voodoo\n"
        "(1,109): end-tag-too-early\n"
        "(1,114): unexpected-end-tag-implies-table-voodoo\n"
        "(1,114): end-tag-too-early\n"
        "(1,119): unexpected-end-tag-implies-table-voodoo\n"
        "(1,119): end-tag-too-early\n"
        "(1,124): unexpected-end-tag-implies-table-voodoo\n"
        "(1,124): end-tag-too-early\n"
        "(1,129): unexpected-end-tag-implies-table-voodoo\n"
        "(1,129): end-tag-too-early\n"
        "(1,136): unexpected-end-tag-in-table-row\n"
        "(1,141): unexpected-end-tag-implies-table-voodoo\n"
        "(1,141): unexpected-end-tag-treated-as\n"
        "(1,145): unexpected-end-tag-implies-table-voodoo\n"
        "(1,145): unexpected-end-tag\n"
        "(1,151): unexpected-end-tag-implies-table-voodoo\n"
        "(1,151): unexpected-end-tag\n"
        "(1,159): unexpected-end-tag-implies-table-voodoo\n"
        "(1,159): unexpected-end-tag\n"
        "(1,166): unexpected-end-tag-implies-table-voodoo\n"
        "(1,166): unexpected-end-tag\n"
        "(1,174): unexpected-end-tag-implies-table-voodoo\n"
        "(1,174): unexpected-end-tag\n"
        "(1,183): unexpected-end-tag-implies-table-voodoo\n"
        "(1,183): unexpected-end-tag\n"
        "(1,196): unexpected-end-tag\n"
        "(1,201): unexpected-end-tag\n"
        "(1,206): unexpected-end-tag\n"
        "(1,214): unexpected-end-tag\n"
        "(1,221): unexpected-end-tag\n"
        "(1,228): unexpected-end-tag\n"
        "(1,236): unexpected-end-tag\n"
        "(1,241): unexpected-end-tag\n"
        "(1,249): unexpected-end-tag\n"
        "(1,255): unexpected-end-tag\n"
        "(1,262): unexpected-end-tag\n"
        "(1,269): unexpected-end-tag\n"
        "(1,280): unexpected-end-tag\n"
        "(1,290): unexpected-end-tag\n"
        "(1,298): unexpected-end-tag\n"
        "(1,307): unexpected-end-tag\n"
        "(1,311): unexpected-end-tag\n"
        "(1,316): unexpected-end-tag\n"
        "(1,321): unexpected-end-tag\n"
        "(1,331): unexpected-end-tag\n"
        "(1,342): unexpected-end-tag\n"
        "(1,350): unexpected-end-tag\n"
        "(1,358): unexpected-end-tag\n"
        "(1,366): unexpected-end-tag\n"
        "(1,376): end-tag-too-early\n"
        "(1,389): end-tag-too-early\n"
        "(1,398): end-tag-too-early\n"
        "(1,404): end-tag-too-early\n"
        "(1,410): end-tag-too-early\n"
        "(1,415): end-tag-too-early\n"
        "(1,426): end-tag-too-early\n"
        "(1,436): end-tag-too-early\n"
        "(1,443): end-tag-too-early\n"
        "(1,448): end-tag-too-early\n"
        "(1,453): end-tag-too-early\n"
        "(1,458): unexpected-end-tag\n"
        "(1,465): unexpected-end-tag\n"
        "(1,471): unexpected-end-tag\n"
        "(1,478): unexpected-end-tag\n"
        "(1,487): end-tag-too-early\n"
        "(1,497): end-tag-too-early\n"
        "(1,506): end-tag-too-early\n"
        "(1,524): expected-eof-but-got-end-tag\n"
        "(1,524): unexpected-end-tag\n"
        "(1,531): unexpected-end-tag\n"
        "(1,540): unexpected-end-tag\n"
        "(1,548): unexpected-end-tag\n"
        "(1,558): unexpected-end-tag\n"
        "(1,568): unexpected-end-tag\n"
        "(1,579): unexpected-end-tag\n"
        "(1,590): unexpected-end-tag\n"
        "(1,601): unexpected-end-tag\n"
        "(1,610): unexpected-end-tag\n"
        "(1,622): unexpected-end-tag\n"
        "(1,633): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <br>\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|     <p>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests10-20") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><frameset><svg><g></g><g></g><p><span>\n"
        "#errors\n"
        "(1,30) unexpected-start-tag-in-frameset\n"
        "(1,33) unexpected-start-tag-in-frameset\n"
        "(1,37) unexpected-end-tag-in-frameset\n"
        "(1,40) unexpected-start-tag-in-frameset\n"
        "(1,44) unexpected-end-tag-in-frameset\n"
        "(1,47) unexpected-start-tag-in-frameset\n"
        "(1,53) unexpected-start-tag-in-frameset\n"
        "(1,53) eof-in-frameset\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <frameset>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests10-21") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><frameset></frameset><svg><g></g><g></g><p><span>\n"
        "#errors\n"
        "(1,41) unexpected-start-tag-after-frameset\n"
        "(1,44) unexpected-start-tag-after-frameset\n"
        "(1,48) unexpected-end-tag-after-frameset\n"
        "(1,51) unexpected-start-tag-after-frameset\n"
        "(1,55) unexpected-end-tag-after-frameset\n"
        "(1,58) unexpected-start-tag-after-frameset\n"
        "(1,64) unexpected-start-tag-after-frameset\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <frameset>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests14-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><html><body><xyz:abc></xyz:abc>\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <xyz:abc>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests14-1") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><html><body><xyz:abc></xyz:abc><span></span>\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <xyz:abc>\n"
        "|     <span>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests14-2") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><html><html abc:def=gh><xyz:abc></xyz:abc>\n"
        "#errors\n"
        "(1,38): non-html-root\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   abc:def=\"gh\"\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <xyz:abc>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests14-4") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><html 123=456>\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   123=\"456\"\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests14-5") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><html 123=456><html 789=012>\n"
        "#errors\n"
        "(1,43): non-html-root\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   123=\"456\"\n"
        "|   789=\"012\"\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests15-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><p><b><i><u></p> <p>X\n"
        "#errors\n"
        "(1,31): unexpected-end-tag\n"
        "(1,36): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <p>\n"
        "|       <b>\n"
        "|         <i>\n"
        "|           <u>\n"
        "|     <b>\n"
        "|       <i>\n"
        "|         <u>\n"
        "|           \" \"\n"
        "|           <p>\n"
        "|             \"X\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests15-1") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<p><b><i><u></p>\n"
        "<p>X\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,16): unexpected-end-tag\n"
        "(2,4): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <p>\n"
        "|       <b>\n"
        "|         <i>\n"
        "|           <u>\n"
        "|     <b>\n"
        "|       <i>\n"
        "|         <u>\n"
        "|           \"\n"
        "\"\n"
        "|           <p>\n"
        "|             \"X\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests16-157") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<script><!--<script --></script>\n"
        "#errors\n"
        "(1,8): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <script>\n"
        "|       \"<!--<script -->\"\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests16-158") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<script><!--<script><\\/script>--></script>\n"
        "#errors\n"
        "(1,8): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <script>\n"
        "|       \"<!--<script><\\/script>-->\"\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests16-159") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<script><!--<script></scr'+'ipt>--></script>\n"
        "#errors\n"
        "(1,8): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <script>\n"
        "|       \"<!--<script></scr'+'ipt>-->\"\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests16-167") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<script><!--<scr'+'ipt></script>--></script>\n"
        "#errors\n"
        "(1,8): expected-doctype-but-got-start-tag\n"
        "(1,44): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <script>\n"
        "|       \"<!--<scr'+'ipt>\"\n"
        "|   <body>\n"
        "|     \"-->\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests16-169") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<style><!--<style></style>--></style>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,37): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <style>\n"
        "|       \"<!--<style>\"\n"
        "|   <body>\n"
        "|     \"-->\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests16-170") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<style><!--</style>X\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <style>\n"
        "|       \"<!--\"\n"
        "|   <body>\n"
        "|     \"X\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests16-171") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<style><!--...</style>...--></style>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,36): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <style>\n"
        "|       \"<!--...\"\n"
        "|   <body>\n"
        "|     \"...-->\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests16-172") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<style><!--<br><html xmlns:v=\"urn:schemas-microsoft-com:vml\"><!--[if !mso]><style></style>X\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <style>\n"
        "|       \"<!--<br><html xmlns:v=\"urn:schemas-microsoft-com:vml\"><!--[if !mso]><style>\"\n"
        "|   <body>\n"
        "|     \"X\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests16-173") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<style><!--...<style><!--...--!></style>--></style>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,51): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <style>\n"
        "|       \"<!--...<style><!--...--!>\"\n"
        "|   <body>\n"
        "|     \"-->\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests16-174") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<style><!--...</style><!-- --><style>@import ...</style>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <style>\n"
        "|       \"<!--...\"\n"
        "|     <!--   -->\n"
        "|     <style>\n"
        "|       \"@import ...\"\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests16-175") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<style>...<style><!--...</style><!-- --></style>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,48): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <style>\n"
        "|       \"...<style><!--...\"\n"
        "|     <!--   -->\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests16-176") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<style>...<!--[if IE]><style>...</style>X\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <style>\n"
        "|       \"...<!--[if IE]><style>...\"\n"
        "|   <body>\n"
        "|     \"X\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests16-177") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<title><!--<title></title>--></title>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,37): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <title>\n"
        "|       \"<!--<title>\"\n"
        "|   <body>\n"
        "|     \"-->\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests16-178") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<title>&lt;/title></title>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <title>\n"
        "|       \"</title>\"\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests16-181") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<noscript><!--<noscript></noscript>--></noscript>\n"
        "#errors\n"
        " * (1,11) missing DOCTYPE\n"
        "#script-off\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <noscript>\n"
        "|       <!-- <noscript></noscript> -->\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests16-183") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<noscript><!--</noscript>X<noscript>--></noscript>\n"
        "#errors\n"
        "(1,10): expected-doctype-but-got-start-tag\n"
        "#script-off\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <noscript>\n"
        "|       <!-- </noscript>X<noscript> -->\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests16-189") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<textarea>&lt;/textarea></textarea>\n"
        "#errors\n"
        "(1,10): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <textarea>\n"
        "|       \"</textarea>\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests18-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<plaintext></plaintext>\n"
        "#errors\n"
        "11: Start tag seen without seeing a doctype first. Expected \342\200\234<!DOCTYPE html>\342\200\235.\n"
        "23: End of file seen and there were open elements.\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <plaintext>\n"
        "|       \"</plaintext>\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests19-43") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html><frameset></frameset></html><!doctype html>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,49): unexpected-doctype\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <frameset>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests19-77") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html>aaa<frameset></frameset>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,19): unexpected-start-tag\n"
        "(1,30): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"aaa\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests19-78") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html> a <frameset></frameset>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,19): unexpected-start-tag\n"
        "(1,30): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"a \"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html>Test\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"Test\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-2") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><td>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,11): unexpected-cell-in-table-body\n"
        "(1,11): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|           <td>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-3") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><td>test</tbody></table>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,11): unexpected-cell-in-table-body\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|           <td>\n"
        "|             \"test\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-4") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<frame>test\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,7): unexpected-start-tag-ignored\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"test\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-5") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><frameset>test\n"
        "#errors\n"
        "(1,29): unexpected-char-in-frameset\n"
        "(1,29): unexpected-char-in-frameset\n"
        "(1,29): unexpected-char-in-frameset\n"
        "(1,29): unexpected-char-in-frameset\n"
        "(1,29): eof-in-frameset\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <frameset>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-8") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><frameset><!DOCTYPE html>\n"
        "#errors\n"
        "(1,40): unexpected-doctype\n"
        "(1,40): eof-in-frameset\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <frameset>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-9") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><font><p><b>test</font>\n"
        "#errors\n"
        "(1,38): adoption-agency-1.3\n"
        "(1,38): adoption-agency-1.3\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <font>\n"
        "|     <p>\n"
        "|       <font>\n"
        "|         <b>\n"
        "|           \"test\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-10") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><dt><div><dd>\n"
        "#errors\n"
        "(1,28): end-tag-too-early\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <dt>\n"
        "|       <div>\n"
        "|     <dd>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-12") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><plaintext><td>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,18): unexpected-start-tag-implies-table-voodoo\n"
        "(1,22): foster-parenting-character-in-table\n"
        "(1,22): foster-parenting-character-in-table\n"
        "(1,22): foster-parenting-character-in-table\n"
        "(1,22): foster-parenting-character-in-table\n"
        "(1,22): eof-in-table\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <plaintext>\n"
        "|       \"<td>\"\n"
        "|     <table>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-13") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<plaintext></plaintext>\n"
        "#errors\n"
        "(1,11): expected-doctype-but-got-start-tag\n"
        "(1,23): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <plaintext>\n"
        "|       \"</plaintext>\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-14") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><table><tr>TEST\n"
        "#errors\n"
        "(1,30): foster-parenting-character-in-table\n"
        "(1,30): foster-parenting-character-in-table\n"
        "(1,30): foster-parenting-character-in-table\n"
        "(1,30): foster-parenting-character-in-table\n"
        "(1,30): eof-in-table\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"TEST\"\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-15") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body t1=1><body t2=2><body t3=3 t4=4>\n"
        "#errors\n"
        "(1,37): unexpected-start-tag\n"
        "(1,53): unexpected-start-tag\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     t1=\"1\"\n"
        "|     t2=\"2\"\n"
        "|     t3=\"3\"\n"
        "|     t4=\"4\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-16") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "</b test\n"
        "#errors\n"
        "(1,8): eof-in-attribute-name\n"
        "(1,8): expected-doctype-but-got-eof\n"
        "#new-errors\n"
        "(1:9) eof-in-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-17") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html></b test<b &=&amp>X\n"
        "#errors\n"
        "(1,24): invalid-character-in-attribute-name\n"
        "(1,32): named-entity-without-semicolon\n"
        "(1,33): attributes-in-end-tag\n"
        "(1,33): unexpected-end-tag-before-html\n"
        "#new-errors\n"
        "(1:24) unexpected-character-in-attribute-name\n"
        "(1:33) missing-semicolon-after-character-reference\n"
        "(1:33) end-tag-with-attributes\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"X\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-19") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "&\n"
        "#errors\n"
        "(1,1): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"&\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-20") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "&#\n"
        "#errors\n"
        "(1,2): expected-numeric-entity\n"
        "(1,2): expected-doctype-but-got-chars\n"
        "#new-errors\n"
        "(1:3) absence-of-digits-in-numeric-character-reference\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"&#\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-24") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "&x-test\n"
        "#errors\n"
        "(1,2): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"&x-test\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-29") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><p></P>X\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <p>\n"
        "|     \"X\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-30") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "&AMP\n"
        "#errors\n"
        "(1,4): named-entity-without-semicolon\n"
        "(1,4): expected-doctype-but-got-chars\n"
        "#new-errors\n"
        "(1:5) missing-semicolon-after-character-reference\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"&\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-31") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "&AMp;\n"
        "#errors\n"
        "(1,3): expected-named-entity\n"
        "(1,3): expected-doctype-but-got-chars\n"
        "#new-errors\n"
        "(1:5) unknown-named-character-reference\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"&AMp;\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-32") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><html><head></head><body><thisISasillyTESTelementNameToMakeSureCrazyTagNamesArePARSEDcorrectLY>\n"
        "#errors\n"
        "(1,110): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <thisisasillytestelementnametomakesurecrazytagnamesareparsedcorrectly>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-35") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><table><caption>test TEST</caption><td>test\n"
        "#errors\n"
        "(1,54): unexpected-cell-in-table-body\n"
        "(1,58): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <caption>\n"
        "|         \"test TEST\"\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|           <td>\n"
        "|             \"test\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-39") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><datalist><option>foo</datalist>bar\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <datalist>\n"
        "|       <option>\n"
        "|         \"foo\"\n"
        "|     \"bar\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-40") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><font><input><input></font>\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <font>\n"
        "|       <input>\n"
        "|       <input>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-44") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html> <!DOCTYPE html>\n"
        "#errors\n"
        "Line: 1 Col: 31 Unexpected DOCTYPE. Ignored.\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-45") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "test\n"
        "test\n"
        "#errors\n"
        "(2,4): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"test\n"
        "test\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-47") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><title>X</title><meta name=z><link rel=foo><style>\n"
        "x { content:\"</style\" } </style>\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <title>\n"
        "|       \"X\"\n"
        "|     <meta>\n"
        "|       name=\"z\"\n"
        "|     <link>\n"
        "|       rel=\"foo\"\n"
        "|     <style>\n"
        "|       \"\n"
        "x { content:\"</style\" } \"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-48") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><select><optgroup></optgroup></select>\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <select>\n"
        "|       <optgroup>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-49") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        " \n"
        " \n"
        "#errors\n"
        "(2,1): expected-doctype-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-50") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html>  <html>\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-51") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><script>\n"
        "</script>  <title>x</title>  </head>\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <script>\n"
        "|       \"\n"
        "\"\n"
        "|     \"  \"\n"
        "|     <title>\n"
        "|       \"x\"\n"
        "|     \"  \"\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-58") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html>X<p/x/y/z>\n"
        "#errors\n"
        "(1,19): unexpected-character-after-solidus-in-tag\n"
        "(1,21): unexpected-character-after-solidus-in-tag\n"
        "(1,23): unexpected-character-after-solidus-in-tag\n"
        "#new-errors\n"
        "(1:20) unexpected-solidus-in-tag\n"
        "(1:22) unexpected-solidus-in-tag\n"
        "(1:24) unexpected-solidus-in-tag\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"X\"\n"
        "|     <p>\n"
        "|       x=\"\"\n"
        "|       y=\"\"\n"
        "|       z=\"\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests2-60") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><table><tr><td></p></table>\n"
        "#errors\n"
        "(1,34): unexpected-end-tag\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|           <td>\n"
        "|             <p>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests20-41") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<p><table></p>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,14): unexpected-end-tag-implies-table-voodoo\n"
        "(1,14): unexpected-end-tag\n"
        "(1,14): eof-in-table\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <p>\n"
        "|       <p>\n"
        "|       <table>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests20-50") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<option><span><option>\n"
        "#errors\n"
        "(1,8): expected-doctype-but-got-start-tag\n"
        "(1,22): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <option>\n"
        "|       <span>\n"
        "|         <option>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests22-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<a><b><big><em><strong><div>X</a>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,33): adoption-agency-1.3\n"
        "(1,33): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <a>\n"
        "|       <b>\n"
        "|         <big>\n"
        "|           <em>\n"
        "|             <strong>\n"
        "|     <big>\n"
        "|       <em>\n"
        "|         <strong>\n"
        "|           <div>\n"
        "|             <a>\n"
        "|               \"X\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests22-1") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<a><b><div id=1><div id=2><div id=3><div id=4><div id=5><div id=6><div id=7><div id=8>A</a>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,91): adoption-agency-1.3\n"
        "(1,91): adoption-agency-1.3\n"
        "(1,91): adoption-agency-1.3\n"
        "(1,91): adoption-agency-1.3\n"
        "(1,91): adoption-agency-1.3\n"
        "(1,91): adoption-agency-1.3\n"
        "(1,91): adoption-agency-1.3\n"
        "(1,91): adoption-agency-1.3\n"
        "(1,91): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <a>\n"
        "|       <b>\n"
        "|     <b>\n"
        "|       <div>\n"
        "|         id=\"1\"\n"
        "|         <a>\n"
        "|         <div>\n"
        "|           id=\"2\"\n"
        "|           <a>\n"
        "|           <div>\n"
        "|             id=\"3\"\n"
        "|             <a>\n"
        "|             <div>\n"
        "|               id=\"4\"\n"
        "|               <a>\n"
        "|               <div>\n"
        "|                 id=\"5\"\n"
        "|                 <a>\n"
        "|                 <div>\n"
        "|                   id=\"6\"\n"
        "|                   <a>\n"
        "|                   <div>\n"
        "|                     id=\"7\"\n"
        "|                     <a>\n"
        "|                     <div>\n"
        "|                       id=\"8\"\n"
        "|                       <a>\n"
        "|                         \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests22-2") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<a><b><div id=1><div id=2><div id=3><div id=4><div id=5><div id=6><div id=7><div id=8><div id=9>A</a>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,101): adoption-agency-1.3\n"
        "(1,101): adoption-agency-1.3\n"
        "(1,101): adoption-agency-1.3\n"
        "(1,101): adoption-agency-1.3\n"
        "(1,101): adoption-agency-1.3\n"
        "(1,101): adoption-agency-1.3\n"
        "(1,101): adoption-agency-1.3\n"
        "(1,101): adoption-agency-1.3\n"
        "(1,101): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <a>\n"
        "|       <b>\n"
        "|     <b>\n"
        "|       <div>\n"
        "|         id=\"1\"\n"
        "|         <a>\n"
        "|         <div>\n"
        "|           id=\"2\"\n"
        "|           <a>\n"
        "|           <div>\n"
        "|             id=\"3\"\n"
        "|             <a>\n"
        "|             <div>\n"
        "|               id=\"4\"\n"
        "|               <a>\n"
        "|               <div>\n"
        "|                 id=\"5\"\n"
        "|                 <a>\n"
        "|                 <div>\n"
        "|                   id=\"6\"\n"
        "|                   <a>\n"
        "|                   <div>\n"
        "|                     id=\"7\"\n"
        "|                     <a>\n"
        "|                     <div>\n"
        "|                       id=\"8\"\n"
        "|                       <a>\n"
        "|                         <div>\n"
        "|                           id=\"9\"\n"
        "|                           \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests22-3") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<a><b><div id=1><div id=2><div id=3><div id=4><div id=5><div id=6><div id=7><div id=8><div id=9><div id=10>A</a>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,112): adoption-agency-1.3\n"
        "(1,112): adoption-agency-1.3\n"
        "(1,112): adoption-agency-1.3\n"
        "(1,112): adoption-agency-1.3\n"
        "(1,112): adoption-agency-1.3\n"
        "(1,112): adoption-agency-1.3\n"
        "(1,112): adoption-agency-1.3\n"
        "(1,112): adoption-agency-1.3\n"
        "(1,112): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <a>\n"
        "|       <b>\n"
        "|     <b>\n"
        "|       <div>\n"
        "|         id=\"1\"\n"
        "|         <a>\n"
        "|         <div>\n"
        "|           id=\"2\"\n"
        "|           <a>\n"
        "|           <div>\n"
        "|             id=\"3\"\n"
        "|             <a>\n"
        "|             <div>\n"
        "|               id=\"4\"\n"
        "|               <a>\n"
        "|               <div>\n"
        "|                 id=\"5\"\n"
        "|                 <a>\n"
        "|                 <div>\n"
        "|                   id=\"6\"\n"
        "|                   <a>\n"
        "|                   <div>\n"
        "|                     id=\"7\"\n"
        "|                     <a>\n"
        "|                     <div>\n"
        "|                       id=\"8\"\n"
        "|                       <a>\n"
        "|                         <div>\n"
        "|                           id=\"9\"\n"
        "|                           <div>\n"
        "|                             id=\"10\"\n"
        "|                             \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests23-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<p><font size=4><font color=red><font size=4><font size=4><font size=4><font size=4><font size=4><font color=red><p>X\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,116): unexpected-end-tag\n"
        "(1,117): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <p>\n"
        "|       <font>\n"
        "|         size=\"4\"\n"
        "|         <font>\n"
        "|           color=\"red\"\n"
        "|           <font>\n"
        "|             size=\"4\"\n"
        "|             <font>\n"
        "|               size=\"4\"\n"
        "|               <font>\n"
        "|                 size=\"4\"\n"
        "|                 <font>\n"
        "|                   size=\"4\"\n"
        "|                   <font>\n"
        "|                     size=\"4\"\n"
        "|                     <font>\n"
        "|                       color=\"red\"\n"
        "|     <p>\n"
        "|       <font>\n"
        "|         color=\"red\"\n"
        "|         <font>\n"
        "|           size=\"4\"\n"
        "|           <font>\n"
        "|             size=\"4\"\n"
        "|             <font>\n"
        "|               size=\"4\"\n"
        "|               <font>\n"
        "|                 color=\"red\"\n"
        "|                 \"X\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests23-1") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<p><font size=4><font size=4><font size=4><font size=4><p>X\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,58): unexpected-end-tag\n"
        "(1,59): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <p>\n"
        "|       <font>\n"
        "|         size=\"4\"\n"
        "|         <font>\n"
        "|           size=\"4\"\n"
        "|           <font>\n"
        "|             size=\"4\"\n"
        "|             <font>\n"
        "|               size=\"4\"\n"
        "|     <p>\n"
        "|       <font>\n"
        "|         size=\"4\"\n"
        "|         <font>\n"
        "|           size=\"4\"\n"
        "|           <font>\n"
        "|             size=\"4\"\n"
        "|             \"X\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests23-2") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<p><font size=4><font size=4><font size=4><font size=\"5\"><font size=4><p>X\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,73): unexpected-end-tag\n"
        "(1,74): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <p>\n"
        "|       <font>\n"
        "|         size=\"4\"\n"
        "|         <font>\n"
        "|           size=\"4\"\n"
        "|           <font>\n"
        "|             size=\"4\"\n"
        "|             <font>\n"
        "|               size=\"5\"\n"
        "|               <font>\n"
        "|                 size=\"4\"\n"
        "|     <p>\n"
        "|       <font>\n"
        "|         size=\"4\"\n"
        "|         <font>\n"
        "|           size=\"4\"\n"
        "|           <font>\n"
        "|             size=\"5\"\n"
        "|             <font>\n"
        "|               size=\"4\"\n"
        "|               \"X\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests23-3") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<p><font size=4 id=a><font size=4 id=b><font size=4><font size=4><p>X\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,68): unexpected-end-tag\n"
        "(1,69): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <p>\n"
        "|       <font>\n"
        "|         id=\"a\"\n"
        "|         size=\"4\"\n"
        "|         <font>\n"
        "|           id=\"b\"\n"
        "|           size=\"4\"\n"
        "|           <font>\n"
        "|             size=\"4\"\n"
        "|             <font>\n"
        "|               size=\"4\"\n"
        "|     <p>\n"
        "|       <font>\n"
        "|         id=\"a\"\n"
        "|         size=\"4\"\n"
        "|         <font>\n"
        "|           id=\"b\"\n"
        "|           size=\"4\"\n"
        "|           <font>\n"
        "|             size=\"4\"\n"
        "|             <font>\n"
        "|               size=\"4\"\n"
        "|               \"X\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests24-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html>&NotEqualTilde;\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"\342\211\202\314\270\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests24-1") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html>&NotEqualTilde;A\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"\342\211\202\314\270A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests24-2") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html>&ThickSpace;\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"\342\201\237\342\200\212\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests24-3") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html>&ThickSpace;A\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"\342\201\237\342\200\212A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests24-4") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html>&NotSubset;\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"\342\212\202\342\203\222\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests24-5") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html>&NotSubset;A\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"\342\212\202\342\203\222A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests24-6") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html>&Gopf;\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"\360\235\224\276\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests25-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><foo>A\n"
        "#errors\n"
        "(1,27): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <foo>\n"
        "|       \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests25-1") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><area>A\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <area>\n"
        "|     \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests25-2") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><base>A\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <base>\n"
        "|     \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests25-3") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><basefont>A\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <basefont>\n"
        "|     \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests25-4") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><bgsound>A\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <bgsound>\n"
        "|     \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests25-5") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><br>A\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <br>\n"
        "|     \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests25-6") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><col>A\n"
        "#errors\n"
        "(1,26): unexpected-start-tag-ignored\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests25-7") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><command>A\n"
        "#errors\n"
        "eof\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <command>\n"
        "|       \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests25-8") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><embed>A\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <embed>\n"
        "|     \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests25-9") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><frame>A\n"
        "#errors\n"
        "(1,28): unexpected-start-tag-ignored\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests25-10") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><hr>A\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <hr>\n"
        "|     \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests25-11") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><img>A\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <img>\n"
        "|     \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests25-12") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><input>A\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <input>\n"
        "|     \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests25-13") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><keygen>A\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <keygen>\n"
        "|     \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests25-14") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><keygen>A</keygen>B\n"
        "#errors\n"
        "33: Stray end tag \342\200\234keygen\342\200\235.\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <keygen>\n"
        "|     \"AB\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests25-15") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "</keygen>A\n"
        "#errors\n"
        "9: End tag seen without seeing a doctype first. Expected \342\200\234<!DOCTYPE html>\342\200\235.\n"
        "9: Stray end tag \342\200\234keygen\342\200\235.\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests25-16") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html></keygen>A\n"
        "#errors\n"
        "24: Stray end tag \342\200\234keygen\342\200\235.\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests25-17") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><head></keygen>A\n"
        "#errors\n"
        "30: Stray end tag \342\200\234keygen\342\200\235.\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests25-18") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><head></head></keygen>A\n"
        "#errors\n"
        "30: Stray end tag \342\200\234keygen\342\200\235.\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests25-19") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body></keygen>A\n"
        "#errors\n"
        "30: Stray end tag \342\200\234keygen\342\200\235.\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests25-20") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><link>A\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <link>\n"
        "|     \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests25-21") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><meta>A\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <meta>\n"
        "|     \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests26-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><a href='#1'><nobr>1<nobr></a><br><a href='#2'><nobr>2<nobr></a><br><a href='#3'><nobr>3<nobr></a>\n"
        "#errors\n"
        "(1,47): unexpected-start-tag-implies-end-tag\n"
        "(1,51): adoption-agency-1.3\n"
        "(1,74): unexpected-start-tag-implies-end-tag\n"
        "(1,74): adoption-agency-1.3\n"
        "(1,81): unexpected-start-tag-implies-end-tag\n"
        "(1,85): adoption-agency-1.3\n"
        "(1,108): unexpected-start-tag-implies-end-tag\n"
        "(1,108): adoption-agency-1.3\n"
        "(1,115): unexpected-start-tag-implies-end-tag\n"
        "(1,119): adoption-agency-1.3\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <a>\n"
        "|       href=\"#1\"\n"
        "|       <nobr>\n"
        "|         \"1\"\n"
        "|       <nobr>\n"
        "|     <nobr>\n"
        "|       <br>\n"
        "|       <a>\n"
        "|         href=\"#2\"\n"
        "|     <a>\n"
        "|       href=\"#2\"\n"
        "|       <nobr>\n"
        "|         \"2\"\n"
        "|       <nobr>\n"
        "|     <nobr>\n"
        "|       <br>\n"
        "|       <a>\n"
        "|         href=\"#3\"\n"
        "|     <a>\n"
        "|       href=\"#3\"\n"
        "|       <nobr>\n"
        "|         \"3\"\n"
        "|       <nobr>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests26-1") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><b><nobr>1<nobr></b><i><nobr>2<nobr></i>3\n"
        "#errors\n"
        "(1,37): unexpected-start-tag-implies-end-tag\n"
        "(1,41): adoption-agency-1.3\n"
        "(1,50): unexpected-start-tag-implies-end-tag\n"
        "(1,50): adoption-agency-1.3\n"
        "(1,57): unexpected-start-tag-implies-end-tag\n"
        "(1,61): adoption-agency-1.3\n"
        "(1,62): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <b>\n"
        "|       <nobr>\n"
        "|         \"1\"\n"
        "|       <nobr>\n"
        "|     <nobr>\n"
        "|       <i>\n"
        "|     <i>\n"
        "|       <nobr>\n"
        "|         \"2\"\n"
        "|       <nobr>\n"
        "|     <nobr>\n"
        "|       \"3\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests26-4") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><b><nobr>1<div><nobr></b><i><nobr>2<nobr></i>3\n"
        "#errors\n"
        "(1,42): unexpected-start-tag-implies-end-tag\n"
        "(1,42): adoption-agency-1.3\n"
        "(1,46): adoption-agency-1.3\n"
        "(1,46): adoption-agency-1.3\n"
        "(1,55): unexpected-start-tag-implies-end-tag\n"
        "(1,55): adoption-agency-1.3\n"
        "(1,62): unexpected-start-tag-implies-end-tag\n"
        "(1,66): adoption-agency-1.3\n"
        "(1,67): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <b>\n"
        "|       <nobr>\n"
        "|         \"1\"\n"
        "|     <div>\n"
        "|       <b>\n"
        "|         <nobr>\n"
        "|         <nobr>\n"
        "|       <nobr>\n"
        "|         <i>\n"
        "|       <i>\n"
        "|         <nobr>\n"
        "|           \"2\"\n"
        "|         <nobr>\n"
        "|       <nobr>\n"
        "|         \"3\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests26-5") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><b><nobr>1<nobr></b><div><i><nobr>2<nobr></i>3\n"
        "#errors\n"
        "(1,37): unexpected-start-tag-implies-end-tag\n"
        "(1,41): adoption-agency-1.3\n"
        "(1,55): unexpected-start-tag-implies-end-tag\n"
        "(1,55): adoption-agency-1.3\n"
        "(1,62): unexpected-start-tag-implies-end-tag\n"
        "(1,66): adoption-agency-1.3\n"
        "(1,67): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <b>\n"
        "|       <nobr>\n"
        "|         \"1\"\n"
        "|       <nobr>\n"
        "|     <div>\n"
        "|       <nobr>\n"
        "|         <i>\n"
        "|       <i>\n"
        "|         <nobr>\n"
        "|           \"2\"\n"
        "|         <nobr>\n"
        "|       <nobr>\n"
        "|         \"3\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests26-6") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><b><nobr>1<nobr><ins></b><i><nobr>\n"
        "#errors\n"
        "(1,37): unexpected-start-tag-implies-end-tag\n"
        "(1,46): adoption-agency-1.3\n"
        "(1,55): unexpected-start-tag-implies-end-tag\n"
        "(1,55): adoption-agency-1.3\n"
        "(1,55): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <b>\n"
        "|       <nobr>\n"
        "|         \"1\"\n"
        "|       <nobr>\n"
        "|         <ins>\n"
        "|     <nobr>\n"
        "|       <i>\n"
        "|     <i>\n"
        "|       <nobr>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests26-7") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><b><nobr>1<ins><nobr></b><i>2\n"
        "#errors\n"
        "(1,42): unexpected-start-tag-implies-end-tag\n"
        "(1,42): adoption-agency-1.3\n"
        "(1,46): adoption-agency-1.3\n"
        "(1,50): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <b>\n"
        "|       <nobr>\n"
        "|         \"1\"\n"
        "|         <ins>\n"
        "|       <nobr>\n"
        "|     <nobr>\n"
        "|       <i>\n"
        "|         \"2\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests26-8") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><b>1<nobr></b><i><nobr>2</i>\n"
        "#errors\n"
        "(1,35): adoption-agency-1.3\n"
        "(1,44): unexpected-start-tag-implies-end-tag\n"
        "(1,44): adoption-agency-1.3\n"
        "(1,49): adoption-agency-1.3\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <b>\n"
        "|       \"1\"\n"
        "|       <nobr>\n"
        "|     <nobr>\n"
        "|       <i>\n"
        "|     <i>\n"
        "|       <nobr>\n"
        "|         \"2\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests26-15") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><body><div><!/div>a\n"
        "#errors\n"
        "(1,28): expected-dashes-or-doctype\n"
        "(1,34): expected-closing-tag-but-got-eof\n"
        "#new-errors\n"
        "(1:29) incorrectly-opened-comment\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       <!-- /div -->\n"
        "|       \"a\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests26-16") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<button><p><button>\n"
        "#errors\n"
        "Line 1 Col 8 Unexpected start tag (button). Expected DOCTYPE.\n"
        "Line 1 Col 19 Unexpected start tag (button) implies end tag (button).\n"
        "Line 1 Col 19 Expected closing tag. Unexpected end of file.\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <button>\n"
        "|       <p>\n"
        "|     <button>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests3-4") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><html><head></head><body><pre>\n"
        "</pre></body></html>\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <pre>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests3-5") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><html><head></head><body><pre>\n"
        "foo</pre></body></html>\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <pre>\n"
        "|       \"foo\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests3-8") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><html><head></head><body><pre>\n"
        "foo\n"
        "</pre></body></html>\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <pre>\n"
        "|       \"foo\n"
        "\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests3-9") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><html><head></head><body><pre>x</pre><span>\n"
        "</span></body></html>\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <pre>\n"
        "|       \"x\"\n"
        "|     <span>\n"
        "|       \"\n"
        "\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests3-10") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><html><head></head><body><pre>x\n"
        "y</pre></body></html>\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <pre>\n"
        "|       \"x\n"
        "y\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests3-11") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><html><head></head><body><pre>x<div>\n"
        "y</pre></body></html>\n"
        "#errors\n"
        "(2,7): end-tag-too-early\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <pre>\n"
        "|       \"x\"\n"
        "|       <div>\n"
        "|         \"\n"
        "y\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests3-13") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><HTML><META><HEAD></HEAD></HTML>\n"
        "#errors\n"
        "(1,33): two-heads-are-not-better-than-one\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <meta>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests3-14") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><HTML><HEAD><head></HEAD></HTML>\n"
        "#errors\n"
        "(1,33): two-heads-are-not-better-than-one\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests3-21") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><html><head></head><body><ul><li><div><p><li></ul></body></html>\n"
        "#errors\n"
        "(1,60): end-tag-too-early\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <ul>\n"
        "|       <li>\n"
        "|         <div>\n"
        "|           <p>\n"
        "|       <li>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests5-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<style> <!-- </style>x\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <style>\n"
        "|       \" <!-- \"\n"
        "|   <body>\n"
        "|     \"x\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests5-1") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<style> <!-- </style> --> </style>x\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,34): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <style>\n"
        "|       \" <!-- \"\n"
        "|     \" \"\n"
        "|   <body>\n"
        "|     \"--> x\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests5-2") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<style> <!--> </style>x\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <style>\n"
        "|       \" <!--> \"\n"
        "|   <body>\n"
        "|     \"x\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests5-3") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<style> <!---> </style>x\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <style>\n"
        "|       \" <!---> \"\n"
        "|   <body>\n"
        "|     \"x\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests5-6") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<script> <!-- </script> --> </script>x\n"
        "#errors\n"
        "(1,8): expected-doctype-but-got-start-tag\n"
        "(1,37): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <script>\n"
        "|       \" <!-- \"\n"
        "|     \" \"\n"
        "|   <body>\n"
        "|     \"--> x\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests5-7") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<title> <!-- </title> --> </title>x\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,34): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <title>\n"
        "|       \" <!-- \"\n"
        "|     \" \"\n"
        "|   <body>\n"
        "|     \"--> x\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests5-9") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<style> <!</-- </style>x\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <style>\n"
        "|       \" <!</-- \"\n"
        "|   <body>\n"
        "|     \"x\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests5-12") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<title>&amp;</title>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <title>\n"
        "|       \"&\"\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests5-13") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<title><!--&amp;--></title>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <title>\n"
        "|       \"<!--&-->\"\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests5-14") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<title><!--</title>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|     <title>\n"
        "|       \"<!--\"\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests6-12") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<form><form>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,12): unexpected-start-tag\n"
        "(1,12): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <form>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests6-13") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<button><button>\n"
        "#errors\n"
        "(1,8): expected-doctype-but-got-start-tag\n"
        "(1,16): unexpected-start-tag-implies-end-tag\n"
        "(1,16): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <button>\n"
        "|     <button>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests6-14") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><tr><td></th>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,20): unexpected-end-tag\n"
        "(1,20): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|           <td>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests6-15") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><caption><td>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,20): unexpected-cell-in-table-body\n"
        "(1,20): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <caption>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|           <td>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests6-16") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><caption><div>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,21): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <caption>\n"
        "|         <div>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests6-18") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><caption><div></caption>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,31): expected-one-end-tag-but-got-another\n"
        "(1,31): eof-in-table\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <caption>\n"
        "|         <div>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests6-19") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><caption></table>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <caption>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests6-21") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><caption></body></col></colgroup></html></tbody></td></tfoot></th></thead></tr>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,23): unexpected-end-tag\n"
        "(1,29): unexpected-end-tag\n"
        "(1,40): unexpected-end-tag\n"
        "(1,47): unexpected-end-tag\n"
        "(1,55): unexpected-end-tag\n"
        "(1,60): unexpected-end-tag\n"
        "(1,68): unexpected-end-tag\n"
        "(1,73): unexpected-end-tag\n"
        "(1,81): unexpected-end-tag\n"
        "(1,86): unexpected-end-tag\n"
        "(1,86): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <caption>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests6-22") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><caption><div></div>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,27): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <caption>\n"
        "|         <div>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests6-23") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><tr><td></body></caption></col></colgroup></html>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,22): unexpected-end-tag\n"
        "(1,32): unexpected-end-tag\n"
        "(1,38): unexpected-end-tag\n"
        "(1,49): unexpected-end-tag\n"
        "(1,56): unexpected-end-tag\n"
        "(1,56): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|           <td>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests6-25") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><colgroup>foo\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,18): foster-parenting-character-in-table\n"
        "(1,19): foster-parenting-character-in-table\n"
        "(1,20): foster-parenting-character-in-table\n"
        "(1,20): eof-in-table\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"foo\"\n"
        "|     <table>\n"
        "|       <colgroup>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests6-27") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><colgroup></col>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,23): no-end-tag\n"
        "(1,23): eof-in-table\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <colgroup>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests6-28") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<frameset><div>\n"
        "#errors\n"
        "(1,10): expected-doctype-but-got-start-tag\n"
        "(1,15): unexpected-start-tag-in-frameset\n"
        "(1,15): eof-in-frameset\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <frameset>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests6-30") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<frameset></div>\n"
        "#errors\n"
        "(1,10): expected-doctype-but-got-start-tag\n"
        "(1,16): unexpected-end-tag-in-frameset\n"
        "(1,16): eof-in-frameset\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <frameset>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests6-32") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><tr><div>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,16): unexpected-start-tag-implies-table-voodoo\n"
        "(1,16): eof-in-table\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests6-35") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><tr><div><td>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,16): foster-parenting-start-tag\n"
        "(1,20): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|           <td>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests6-37") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><tbody></thead>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,22): unexpected-end-tag-in-table-body\n"
        "(1,22): eof-in-table\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <tbody>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests6-39") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><tbody></body></caption></col></colgroup></html></td></th></tr>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,21): unexpected-end-tag-in-table-body\n"
        "(1,31): unexpected-end-tag-in-table-body\n"
        "(1,37): unexpected-end-tag-in-table-body\n"
        "(1,48): unexpected-end-tag-in-table-body\n"
        "(1,55): unexpected-end-tag-in-table-body\n"
        "(1,60): unexpected-end-tag-in-table-body\n"
        "(1,65): unexpected-end-tag-in-table-body\n"
        "(1,70): unexpected-end-tag-in-table-body\n"
        "(1,70): eof-in-table\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <tbody>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests6-40") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><tbody></div>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,20): unexpected-end-tag-implies-table-voodoo\n"
        "(1,20): end-tag-too-early\n"
        "(1,20): eof-in-table\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <tbody>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests6-41") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><table>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,14): unexpected-start-tag-implies-end-tag\n"
        "(1,14): eof-in-table\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|     <table>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests6-42") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table></body></caption></col></colgroup></html></tbody></td></tfoot></th></thead></tr>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,14): unexpected-end-tag\n"
        "(1,24): unexpected-end-tag\n"
        "(1,30): unexpected-end-tag\n"
        "(1,41): unexpected-end-tag\n"
        "(1,48): unexpected-end-tag\n"
        "(1,56): unexpected-end-tag\n"
        "(1,61): unexpected-end-tag\n"
        "(1,69): unexpected-end-tag\n"
        "(1,74): unexpected-end-tag\n"
        "(1,82): unexpected-end-tag\n"
        "(1,87): unexpected-end-tag\n"
        "(1,87): eof-in-table\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests6-47") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<param><frameset></frameset>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,17): unexpected-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <frameset>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests6-48") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<source><frameset></frameset>\n"
        "#errors\n"
        "(1,8): expected-doctype-but-got-start-tag\n"
        "(1,18): unexpected-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <frameset>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests6-49") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<track><frameset></frameset>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,17): unexpected-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <frameset>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests7-25") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE hTmL><html></html>\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests7-26") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE HTML><html></html>\n"
        "#errors\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests7-28") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div><p>a</x> b\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,13): unexpected-end-tag\n"
        "(1,15): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       <p>\n"
        "|         \"a b\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests7-29") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><tr><td><code></code> </table>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|           <td>\n"
        "|             <code>\n"
        "|             \" \"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests7-31") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "A<table><tr> B</tr> B</table>\n"
        "#errors\n"
        "(1,1): expected-doctype-but-got-chars\n"
        "(1,13): foster-parenting-character\n"
        "(1,14): foster-parenting-character\n"
        "(1,20): foster-parenting-character\n"
        "(1,21): foster-parenting-character\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"A B B\"\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests7-32") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "A<table><tr> B</tr> </em>C</table>\n"
        "#errors\n"
        "(1,1): expected-doctype-but-got-chars\n"
        "(1,13): foster-parenting-character\n"
        "(1,14): foster-parenting-character\n"
        "(1,25): unexpected-end-tag\n"
        "(1,25): unexpected-end-tag-in-special-element\n"
        "(1,26): foster-parenting-character\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"A BC\"\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|         \" \"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests8-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div>\n"
        "<div></div>\n"
        "</span>x\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(3,7): unexpected-end-tag\n"
        "(3,8): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \"\n"
        "\"\n"
        "|       <div>\n"
        "|       \"\n"
        "x\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests8-1") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div>x<div></div>\n"
        "</span>x\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(2,7): unexpected-end-tag\n"
        "(2,8): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \"x\"\n"
        "|       <div>\n"
        "|       \"\n"
        "x\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests8-2") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div>x<div></div>x</span>x\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,25): unexpected-end-tag\n"
        "(1,26): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \"x\"\n"
        "|       <div>\n"
        "|       \"xx\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests8-3") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div>x<div></div>y</span>z\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,25): unexpected-end-tag\n"
        "(1,26): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \"x\"\n"
        "|       <div>\n"
        "|       \"yz\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests8-4") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><div>x<div></div>x</span>x\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,12): foster-parenting-start-tag\n"
        "(1,13): foster-parenting-character\n"
        "(1,18): foster-parenting-start-tag\n"
        "(1,24): foster-parenting-end-tag\n"
        "(1,25): foster-parenting-start-tag\n"
        "(1,32): foster-parenting-end-tag\n"
        "(1,32): unexpected-end-tag\n"
        "(1,33): foster-parenting-character\n"
        "(1,33): eof-in-table\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \"x\"\n"
        "|       <div>\n"
        "|       \"xx\"\n"
        "|     <table>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests8-5") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><li><li></table>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,11): foster-parenting-start-tag\n"
        "(1,15): foster-parenting-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <li>\n"
        "|     <li>\n"
        "|     <table>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests8-6") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "x<table>x\n"
        "#errors\n"
        "(1,1): expected-doctype-but-got-chars\n"
        "(1,9): foster-parenting-character\n"
        "(1,9): eof-in-table\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"xx\"\n"
        "|     <table>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests8-7") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "x<table><table>x\n"
        "#errors\n"
        "(1,1): expected-doctype-but-got-chars\n"
        "(1,15): unexpected-start-tag-implies-end-tag\n"
        "(1,16): foster-parenting-character\n"
        "(1,16): eof-in-table\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"x\"\n"
        "|     <table>\n"
        "|     \"x\"\n"
        "|     <table>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests8-8") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<b>a<div></div><div></b>y\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,24): adoption-agency-1.3\n"
        "(1,25): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <b>\n"
        "|       \"a\"\n"
        "|       <div>\n"
        "|     <div>\n"
        "|       <b>\n"
        "|       \"y\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests9-21") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><frameset><math><mi></mi><mi></mi><p><span>\n"
        "#errors\n"
        "(1,31) unexpected-start-tag-in-frameset\n"
        "(1,35) unexpected-start-tag-in-frameset\n"
        "(1,40) unexpected-end-tag-in-frameset\n"
        "(1,44) unexpected-start-tag-in-frameset\n"
        "(1,49) unexpected-end-tag-in-frameset\n"
        "(1,52) unexpected-start-tag-in-frameset\n"
        "(1,58) unexpected-start-tag-in-frameset\n"
        "(1,58) eof-in-frameset\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <frameset>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tests9-22") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<!DOCTYPE html><frameset></frameset><math><mi></mi><mi></mi><p><span>\n"
        "#errors\n"
        "(1,42) unexpected-start-tag-after-frameset\n"
        "(1,46) unexpected-start-tag-after-frameset\n"
        "(1,51) unexpected-end-tag-after-frameset\n"
        "(1,55) unexpected-start-tag-after-frameset\n"
        "(1,60) unexpected-end-tag-after-frameset\n"
        "(1,63) unexpected-start-tag-after-frameset\n"
        "(1,69) unexpected-start-tag-after-frameset\n"
        "#document\n"
        "| <!DOCTYPE html>\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <frameset>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tricky01-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<b><p>Bold </b> Not bold</p>\n"
        "Also not bold.\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,15): adoption-agency-1.3\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <b>\n"
        "|     <p>\n"
        "|       <b>\n"
        "|         \"Bold \"\n"
        "|       \" Not bold\"\n"
        "|     \"\n"
        "Also not bold.\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tricky01-1") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html>\n"
        "<font color=red><i>Italic and Red<p>Italic and Red </font> Just italic.</p> Italic only.</i> Plain\n"
        "<p>I should not be red. <font color=red>Red. <i>Italic and red.</p>\n"
        "<p>Italic and red. </i> Red.</font> I should not be red.</p>\n"
        "<b>Bold <i>Bold and italic</b> Only Italic </i> Plain\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(2,58): adoption-agency-1.3\n"
        "(3,67): unexpected-end-tag\n"
        "(4,23): adoption-agency-1.3\n"
        "(4,35): adoption-agency-1.3\n"
        "(5,30): adoption-agency-1.3\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <font>\n"
        "|       color=\"red\"\n"
        "|       <i>\n"
        "|         \"Italic and Red\"\n"
        "|     <i>\n"
        "|       <p>\n"
        "|         <font>\n"
        "|           color=\"red\"\n"
        "|           \"Italic and Red \"\n"
        "|         \" Just italic.\"\n"
        "|       \" Italic only.\"\n"
        "|     \" Plain\n"
        "\"\n"
        "|     <p>\n"
        "|       \"I should not be red. \"\n"
        "|       <font>\n"
        "|         color=\"red\"\n"
        "|         \"Red. \"\n"
        "|         <i>\n"
        "|           \"Italic and red.\"\n"
        "|     <font>\n"
        "|       color=\"red\"\n"
        "|       <i>\n"
        "|         \"\n"
        "\"\n"
        "|     <p>\n"
        "|       <font>\n"
        "|         color=\"red\"\n"
        "|         <i>\n"
        "|           \"Italic and red. \"\n"
        "|         \" Red.\"\n"
        "|       \" I should not be red.\"\n"
        "|     \"\n"
        "\"\n"
        "|     <b>\n"
        "|       \"Bold \"\n"
        "|       <i>\n"
        "|         \"Bold and italic\"\n"
        "|     <i>\n"
        "|       \" Only Italic \"\n"
        "|     \" Plain\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tricky01-2") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html><body>\n"
        "<p><font size=\"7\">First paragraph.</p>\n"
        "<p>Second paragraph.</p></font>\n"
        "<b><p><i>Bold and Italic</b> Italic</p>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(2,38): unexpected-end-tag\n"
        "(4,28): adoption-agency-1.3\n"
        "(4,28): adoption-agency-1.3\n"
        "(4,39): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"\n"
        "\"\n"
        "|     <p>\n"
        "|       <font>\n"
        "|         size=\"7\"\n"
        "|         \"First paragraph.\"\n"
        "|     <font>\n"
        "|       size=\"7\"\n"
        "|       \"\n"
        "\"\n"
        "|       <p>\n"
        "|         \"Second paragraph.\"\n"
        "|     \"\n"
        "\"\n"
        "|     <b>\n"
        "|     <p>\n"
        "|       <b>\n"
        "|         <i>\n"
        "|           \"Bold and Italic\"\n"
        "|       <i>\n"
        "|         \" Italic\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tricky01-3") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html>\n"
        "<dl>\n"
        "<dt><b>Boo\n"
        "<dd>Goo?\n"
        "</dl>\n"
        "</html>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(4,4): end-tag-too-early\n"
        "(5,5): end-tag-too-early\n"
        "(6,7): expected-one-end-tag-but-got-another\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <dl>\n"
        "|       \"\n"
        "\"\n"
        "|       <dt>\n"
        "|         <b>\n"
        "|           \"Boo\n"
        "\"\n"
        "|       <dd>\n"
        "|         <b>\n"
        "|           \"Goo?\n"
        "\"\n"
        "|     <b>\n"
        "|       \"\n"
        "\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tricky01-6") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><tr><p><a><p>You should see this text.\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,14): unexpected-start-tag-implies-table-voodoo\n"
        "(1,17): unexpected-start-tag-implies-table-voodoo\n"
        "(1,20): unexpected-start-tag-implies-table-voodoo\n"
        "(1,20): closing-non-current-p-element\n"
        "(1,21): foster-parenting-character\n"
        "(1,22): foster-parenting-character\n"
        "(1,23): foster-parenting-character\n"
        "(1,24): foster-parenting-character\n"
        "(1,25): foster-parenting-character\n"
        "(1,26): foster-parenting-character\n"
        "(1,27): foster-parenting-character\n"
        "(1,28): foster-parenting-character\n"
        "(1,29): foster-parenting-character\n"
        "(1,30): foster-parenting-character\n"
        "(1,31): foster-parenting-character\n"
        "(1,32): foster-parenting-character\n"
        "(1,33): foster-parenting-character\n"
        "(1,34): foster-parenting-character\n"
        "(1,35): foster-parenting-character\n"
        "(1,36): foster-parenting-character\n"
        "(1,37): foster-parenting-character\n"
        "(1,38): foster-parenting-character\n"
        "(1,39): foster-parenting-character\n"
        "(1,40): foster-parenting-character\n"
        "(1,41): foster-parenting-character\n"
        "(1,42): foster-parenting-character\n"
        "(1,43): foster-parenting-character\n"
        "(1,44): foster-parenting-character\n"
        "(1,45): foster-parenting-character\n"
        "(1,45): eof-in-table\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <p>\n"
        "|       <a>\n"
        "|     <p>\n"
        "|       <a>\n"
        "|         \"You should see this text.\"\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-tricky01-7") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<TABLE>\n"
        "<TR>\n"
        "<CENTER><CENTER><TD></TD></TR><TR>\n"
        "<FONT>\n"
        "<TABLE><tr></tr></TABLE>\n"
        "</P>\n"
        "<a></font><font></a>\n"
        "This page contains an insanely badly-nested tag sequence.\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(3,8): unexpected-start-tag-implies-table-voodoo\n"
        "(3,16): unexpected-start-tag-implies-table-voodoo\n"
        "(4,6): unexpected-start-tag-implies-table-voodoo\n"
        "(4,6): unexpected character token in table (the newline)\n"
        "(5,7): unexpected-start-tag-implies-end-tag\n"
        "(6,4): unexpected p end tag\n"
        "(7,10): adoption-agency-1.3\n"
        "(7,20): adoption-agency-1.3\n"
        "(8,57): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <center>\n"
        "|       <center>\n"
        "|     <font>\n"
        "|       \"\n"
        "\"\n"
        "|     <table>\n"
        "|       \"\n"
        "\"\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|           \"\n"
        "\"\n"
        "|           <td>\n"
        "|         <tr>\n"
        "|           \"\n"
        "\"\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|     <font>\n"
        "|       \"\n"
        "\"\n"
        "|       <p>\n"
        "|       \"\n"
        "\"\n"
        "|       <a>\n"
        "|     <a>\n"
        "|       <font>\n"
        "|     <font>\n"
        "|       \"\n"
        "This page contains an insanely badly-nested tag sequence.\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit01-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "Test\n"
        "#errors\n"
        "(1,4): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"Test\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit01-1") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div></div>\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit01-2") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div>Test</div>\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \"Test\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit01-3") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<di\n"
        "#errors\n"
        "(1,3): eof-in-tag-name\n"
        "(1,3): expected-doctype-but-got-eof\n"
        "#new-errors\n"
        "(1:4) eof-in-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit01-4") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div>Hello</div>\n"
        "<script>\n"
        "console.log(\"PASS\");\n"
        "</script>\n"
        "<div>Bye</div>\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       \"Hello\"\n"
        "|     \"\n"
        "\"\n"
        "|     <script>\n"
        "|       \"\n"
        "console.log(\"PASS\");\n"
        "\"\n"
        "|     \"\n"
        "\"\n"
        "|     <div>\n"
        "|       \"Bye\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit01-5") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div foo=\"bar\">Hello</div>\n"
        "#errors\n"
        "(1,15): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       foo=\"bar\"\n"
        "|       \"Hello\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit01-7") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<foo bar=\"baz\"></foo><potato quack=\"duck\"></potato>\n"
        "#errors\n"
        "(1,15): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <foo>\n"
        "|       bar=\"baz\"\n"
        "|     <potato>\n"
        "|       quack=\"duck\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit01-8") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<foo bar=\"baz\"><potato quack=\"duck\"></potato></foo>\n"
        "#errors\n"
        "(1,15): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <foo>\n"
        "|       bar=\"baz\"\n"
        "|       <potato>\n"
        "|         quack=\"duck\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit01-9") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<foo></foo bar=\"baz\"><potato></potato quack=\"duck\">\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,21): attributes-in-end-tag\n"
        "(1,51): attributes-in-end-tag\n"
        "#new-errors\n"
        "(1:21) end-tag-with-attributes\n"
        "(1:51) end-tag-with-attributes\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <foo>\n"
        "|     <potato>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit01-10") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "</ tttt>\n"
        "#errors\n"
        "(1,2): expected-closing-tag-but-got-char\n"
        "(1,8): expected-doctype-but-got-eof\n"
        "#new-errors\n"
        "(1:3) invalid-first-character-of-tag-name\n"
        "#document\n"
        "| <!--  tttt -->\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit01-11") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div FOO ><img><img></div>\n"
        "#errors\n"
        "(1,10): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       foo=\"\"\n"
        "|       <img>\n"
        "|       <img>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit01-12") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<p>Test</p<p>Test2</p>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,13): unexpected-end-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <p>\n"
        "|       \"TestTest2\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit01-13") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<rdar://problem/6869687>\n"
        "#errors\n"
        "(1,7): unexpected-character-after-solidus-in-tag\n"
        "(1,8): unexpected-character-after-solidus-in-tag\n"
        "(1,16): unexpected-character-after-solidus-in-tag\n"
        "(1,24): expected-doctype-but-got-start-tag\n"
        "(1,24): expected-closing-tag-but-got-eof\n"
        "#new-errors\n"
        "(1:8) unexpected-solidus-in-tag\n"
        "(1:9) unexpected-solidus-in-tag\n"
        "(1:17) unexpected-solidus-in-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <rdar:>\n"
        "|       6869687=\"\"\n"
        "|       problem=\"\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit01-14") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<A>test< /A>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,8): expected-tag-name\n"
        "(1,12): expected-closing-tag-but-got-eof\n"
        "#new-errors\n"
        "(1:9) invalid-first-character-of-tag-name\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <a>\n"
        "|       \"test< /A>\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit01-15") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "&lt;\n"
        "#errors\n"
        "(1,4): expected-doctype-but-got-chars\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"<\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit01-18") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<bdy><br foo=\"bar\"></body>\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,26): expected-one-end-tag-but-got-another\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <bdy>\n"
        "|       <br>\n"
        "|         foo=\"bar\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit01-28") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html><body><ruby><div><rp>xx</rp></div></ruby></body></html>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,27): XXX-undefined-error\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <ruby>\n"
        "|       <div>\n"
        "|         <rp>\n"
        "|           \"xx\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit01-29") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html><body><ruby><div><rt>xx</rt></div></ruby></body></html>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,27): XXX-undefined-error\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <ruby>\n"
        "|       <div>\n"
        "|         <rt>\n"
        "|           \"xx\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit01-32") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<dd><dd><dt><dt><dd><li><li>\n"
        "#errors\n"
        "(1,4): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <dd>\n"
        "|     <dd>\n"
        "|     <dt>\n"
        "|     <dt>\n"
        "|     <dd>\n"
        "|       <li>\n"
        "|       <li>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit01-33") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div><b></div><div><nobr>a<nobr>\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "(1,14): end-tag-too-early\n"
        "(1,32): unexpected-start-tag-implies-end-tag\n"
        "(1,32): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       <b>\n"
        "|     <div>\n"
        "|       <b>\n"
        "|         <nobr>\n"
        "|           \"a\"\n"
        "|         <nobr>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit01-34") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<head></head>\n"
        "<body></body>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   \"\n"
        "\"\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit01-44") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<img <=\"\" FAIL>\n"
        "#errors\n"
        "(1,6): invalid-character-in-attribute-name\n"
        "(1,15): expected-doctype-but-got-start-tag\n"
        "#new-errors\n"
        "(1:6) unexpected-character-in-attribute-name\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <img>\n"
        "|       <=\"\"\n"
        "|       fail=\"\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit01-45") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<ul><li><div id='foo'/>A</li><li>B<div>C</div></li></ul>\n"
        "#errors\n"
        "(1,4): expected-doctype-but-got-start-tag\n"
        "(1,23): non-void-element-with-trailing-solidus\n"
        "(1,29): end-tag-too-early\n"
        "#new-errors\n"
        "(1:9-1:24) non-void-html-element-start-tag-with-trailing-solidus\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <ul>\n"
        "|       <li>\n"
        "|         <div>\n"
        "|           id=\"foo\"\n"
        "|           \"A\"\n"
        "|       <li>\n"
        "|         \"B\"\n"
        "|         <div>\n"
        "|           \"C\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit02-0") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<foo bar=qux/>\n"
        "#errors\n"
        "(1,14): expected-doctype-but-got-start-tag\n"
        "(1,14): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <foo>\n"
        "|       bar=\"qux/\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit02-2") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<p id=\"status\"><noscript><strong>A</strong></noscript><span>B</span></p>\n"
        "#errors\n"
        "(1,15): expected-doctype-but-got-start-tag\n"
        "#script-off\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <p>\n"
        "|       id=\"status\"\n"
        "|       <noscript>\n"
        "|         <strong>\n"
        "|           \"A\"\n"
        "|       <span>\n"
        "|         \"B\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit02-3") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div><sarcasm><div></div></sarcasm></div>\n"
        "#errors\n"
        "(1,5): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       <sarcasm>\n"
        "|         <div>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit02-4") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<html><body><img src=\"\" border=\"0\" alt=\"><div>A</div></body></html>\n"
        "#errors\n"
        "(1,6): expected-doctype-but-got-start-tag\n"
        "(1,67): eof-in-attribute-value-double-quote\n"
        "#new-errors\n"
        "(1:68) eof-in-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit02-5") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><td></tbody>A\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,11): unexpected-cell-in-table-body\n"
        "(1,20): foster-parenting-character\n"
        "(1,20): eof-in-table\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     \"A\"\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|           <td>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit02-6") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><td></thead>A\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,11): unexpected-cell-in-table-body\n"
        "(1,19): XXX-undefined-error\n"
        "(1,20): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|           <td>\n"
        "|             \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit02-7") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><td></tfoot>A\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,11): unexpected-cell-in-table-body\n"
        "(1,19): XXX-undefined-error\n"
        "(1,20): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|           <td>\n"
        "|             \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit02-8") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><thead><td></tbody>A\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "(1,18): unexpected-cell-in-table-body\n"
        "(1,26): XXX-undefined-error\n"
        "(1,27): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <thead>\n"
        "|         <tr>\n"
        "|           <td>\n"
        "|             \"A\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit02-9") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<legend>test</legend>\n"
        "#errors\n"
        "(1,7): expected-doctype-but-got-start-tag\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <legend>\n"
        "|       \"test\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit02-12") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<b><em><foo><foo><aside></b>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,28): adoption-agency-9\n"
        "(1,29): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <b>\n"
        "|       <em>\n"
        "|         <foo>\n"
        "|           <foo>\n"
        "|     <em>\n"
        "|       <aside>\n"
        "|         <b>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit02-13") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<b><em><foo><foo><aside></b></em>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,28): adoption-agency-9\n"
        "(1,33): adoption-agency-9\n"
        "(1,34): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <b>\n"
        "|       <em>\n"
        "|         <foo>\n"
        "|           <foo>\n"
        "|     <em>\n"
        "|     <aside>\n"
        "|       <em>\n"
        "|         <b>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit02-14") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<b><em><foo><foo><foo><aside></b>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,33): adoption-agency-9\n"
        "(1,34): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <b>\n"
        "|       <em>\n"
        "|         <foo>\n"
        "|           <foo>\n"
        "|             <foo>\n"
        "|     <aside>\n"
        "|       <b>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit02-15") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<b><em><foo><foo><foo><aside></b></em>\n"
        "#errors\n"
        "(1,3): expected-doctype-but-got-start-tag\n"
        "(1,33): adoption-agency-9\n"
        "(1,38): adoption-agency-9\n"
        "(1,39): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <b>\n"
        "|       <em>\n"
        "|         <foo>\n"
        "|           <foo>\n"
        "|             <foo>\n"
        "|     <aside>\n"
        "|       <b>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit02-21") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "</foreignObject><plaintext><div>foo</div>\n"
        "#errors\n"
        "(1,16): expected-doctype-but-got-end-tag\n"
        "(1,16): unexpected-end-tag-before-html\n"
        "(1,42): expected-closing-tag-but-got-eof\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <plaintext>\n"
        "|       \"<div>foo</div>\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit02-25") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<select><hr>\n"
        "#errors\n"
        "1:1: ERROR: Expected a doctype token\n"
        "1:13: ERROR: Premature end of file. Currently open tags: html, body, select.\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <select>\n"
        "|       <hr>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit02-30") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<table><tr><td><select><hr>\n"
        "#errors\n"
        "1:1: ERROR: Expected a doctype token\n"
        "1:28: ERROR: Premature end of file. Currently open tags: html, body, table, tbody, tr, td, select.\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <table>\n"
        "|       <tbody>\n"
        "|         <tr>\n"
        "|           <td>\n"
        "|             <select>\n"
        "|               <hr>\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit02-35") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<select><div><i></div><option>option\n"
        "#errors\n"
        "1:1: ERROR: Expected a doctype token\n"
        "1:17: ERROR: End tag 'div' isn't allowed here. Currently open tags: html, body, select, div, i.\n"
        "1:37: ERROR: Premature end of file. Currently open tags: html, body, select, option, i.\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <select>\n"
        "|       <div>\n"
        "|         <i>\n"
        "|       <i>\n"
        "|         <option>\n"
        "|           \"option\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit02-36") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<div><i></div><option>option\n"
        "#errors\n"
        "1:1: ERROR: Expected a doctype token\n"
        "1:9: ERROR: End tag 'div' isn't allowed here. Currently open tags: html, body, div, i.\n"
        "1:29: ERROR: Premature end of file. Currently open tags: html, body, i, option.\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <div>\n"
        "|       <i>\n"
        "|     <i>\n"
        "|       <option>\n"
        "|         \"option\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit02-37") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<select><div>div 1</div><button>button</button><div>div 2</div><datalist><option>option</option></datalist><div>div 3</div></select>\n"
        "#errors\n"
        "1:1: ERROR: Expected a doctype token\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <select>\n"
        "|       <div>\n"
        "|         \"div 1\"\n"
        "|       <button>\n"
        "|         \"button\"\n"
        "|       <div>\n"
        "|         \"div 2\"\n"
        "|       <datalist>\n"
        "|         <option>\n"
        "|           \"option\"\n"
        "|       <div>\n"
        "|         \"div 3\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit02-38") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<select><button>button</select>\n"
        "#errors\n"
        "1:1: ERROR: Expected a doctype token\n"
        "1:23: ERROR: End tag 'select' isn't allowed here. Currently open tags: html, body, select, button.\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <select>\n"
        "|       <button>\n"
        "|         \"button\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit02-39") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<select><datalist>datalist</select>\n"
        "#errors\n"
        "1:1: ERROR: Expected a doctype token\n"
        "1:27: ERROR: End tag 'select' isn't allowed here. Currently open tags: html, body, select, datalist.\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <select>\n"
        "|       <datalist>\n"
        "|         \"datalist\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

test$("html5lib-webkit02-42") {
    auto result = try$(Html5LibTest::run(
        "#data\n"
        "<select><div><option><img>option</option></div></select>\n"
        "#errors\n"
        "1:1: ERROR: Expected a doctype token\n"
        "#document\n"
        "| <html>\n"
        "|   <head>\n"
        "|   <body>\n"
        "|     <select>\n"
        "|       <div>\n"
        "|         <option>\n"
        "|           <img>\n"
        "|           \"option\"\n"
        ""s
    ));

    expect$(result.passed);

    return Ok();
}

} // namespace Vaev::Html::Tests

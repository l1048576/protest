cxxflags_base=
# -pedantic: 標準でない文法をエラーとする(コンパイラ独自拡張を無効にする)。
cxxflags_base += -pedantic

#
# Set C++ warning flags.
#
cxxflags_warning = -Wall -Wextra
# -Wformat=2: -Wformat -Wformat-nonliteral -Wformat-security -Wformat-y2k に等しい。
cxxflags_warning += -Wformat=2
# -Wfloat-equal: 浮動小数点数を等価比較している場合に警告する。
cxxflags_warning += -Wfloat-equal
# -Winit-self: 変数の初期化にそれ自身を必要としている(int i=i;等)場合に警告する。
cxxflags_warning += -Winit-self
# -Wdisabled-optimization: コンパイラに負荷がかかりすぎ、最適化を断念した場合に警告する。
cxxflags_warning += -Wdisabled-optimization
# -Wswitch-enum: switchのインデックスで列挙型の項目すべてが列挙されていない場合に警告する。
cxxflags_warning += -Wswitch-enum
# -Wpointer-arith: 関数型やvoidのサイズに依存するものを全て警告。
cxxflags_warning += -Wpointer-arith
# -Wformat-nonliteral: printf系やscanf系の引数のフォーマット文字列がリテラルでない場合に警告する。
#                      フォーマットストリング攻撃への脆弱性を防ぐのに役立つ。
cxxflags_warning += -Wformat-nonliteral

if DEBUG

cxxflags_warning_dev =
# -Wcast-align: より厳密なアライメントを要求する型へのポインタへのキャスト(char*からfloat*等)を警告する。
cxxflags_warning_dev += -Wcast-align
# -Wwrite-strings: const char *がchar*に変換される場合に警告する。
cxxflags_warning_dev += -Wwrite-strings
# -Wpadded: パディングされた場合に警告する。
cxxflags_warning_dev += -Wpadded
# -Wunreachable-code: 実行されないコードがあるときに警告。
cxxflags_warning_dev += -Wunreachable-code
# -Wabi: コンパイラが生成したコードがおそらくベンダー非依存なC++ ABIと互換性のない場合に警告する。
cxxflags_warning_dev += -Wabi
# -Winline: inline指定されたのにインライン展開されなかった場合に警告する。
cxxflags_warning_dev += -Winline

cxxflags_warning += ${cxxflags_warning_dev}

else !DEBUG
# automakeのファイルでelse-endifを使う場合は、endifの直後で
# 対応するelseの条件を指定してやらなければいけない。
# elseの条件がifと非対称だった(否定でなかった)場合、エラーとなる。

cxxflags_base += ${cxxflags_warning}
endif !DEBUG

TESTS = protest_test
check_PROGRAMS = protest_test

lib_LTLIBRARIES = libprotest.la

AM_CXXFLAGS = ${cxxflags_base}
AM_CPPFLAGS = -Iinclude

protest_test_SOURCES = protest_test.cpp
protest_test_LDADD = libprotest.la

libprotest_la_SOURCES = protest.cpp

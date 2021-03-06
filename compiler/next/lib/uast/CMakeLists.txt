# Copyright 2021 Hewlett Packard Enterprise Development LP
# Other additional copyright holders may be indicated within.
#
# The entirety of this work is licensed under the Apache License,
# Version 2.0 (the "License"); you may not use this file except
# in compliance with the License.
#
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

target_sources(libchplcomp-obj
               PRIVATE

               AggregateDecl.cpp
               ASTList.cpp
               ASTNode.cpp
               ASTTag.cpp
               As.cpp
               Begin.cpp
               Block.cpp
               BracketLoop.cpp
               Break.cpp
               Builder.cpp
               BytesLiteral.cpp
               Call.cpp
               Class.cpp
               Cobegin.cpp
               Coforall.cpp
               Comment.cpp
               Conditional.cpp
               Continue.cpp
               CStringLiteral.cpp
               Decl.cpp
               Defer.cpp
               Delete.cpp
               Dot.cpp
               DoWhile.cpp
               Enum.cpp
               EnumElement.cpp
               ErroneousExpression.cpp
               Expression.cpp
               FnCall.cpp
               For.cpp
               Forall.cpp
               Foreach.cpp
               Formal.cpp
               Function.cpp
               Identifier.cpp
               ImagLiteral.cpp
               IndexableLoop.cpp
               IntLiteral.cpp
               Label.cpp
               Literal.cpp
               Local.cpp
               Loop.cpp
               Module.cpp
               MultiDecl.cpp
               NamedDecl.cpp
               New.cpp
               NumericLiteral.cpp
               On.cpp
               OpCall.cpp
               RealLiteral.cpp
               Record.cpp
               Return.cpp
               Serial.cpp
               SimpleBlockLike.cpp
               StringLikeLiteral.cpp
               StringLiteral.cpp
               TaskVar.cpp
               TupleDecl.cpp
               TypeDecl.cpp
               UintLiteral.cpp
               Use.cpp
               UseClause.cpp
               Union.cpp
               Variable.cpp
               VarLikeDecl.cpp
               Visitor.cpp
               While.cpp
               WithClause.cpp
               Yield.cpp
               Zip.cpp

              )

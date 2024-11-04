#include "ITBParser.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "regex"

bool UITBParser::ParseFile(const FString& FilePath) {
    FString Data;
    if (!FFileHelper::LoadFileToString(Data, *FilePath)) {
        UE_LOG(LogTemp, Error, TEXT("Failed to load file: %s"), *FilePath);
        return false;
    }

    TArray<FString> Tokens;
    Tokenize(Data, Tokens);
    ParseTokens(Tokens);

    return true;
}

void UITBParser::Tokenize(const FString& Data, TArray<FString>& Tokens) {
    const FRegexPattern TokenPattern(TEXT(R"(\[|\]|"[^"]*"|[\w@.-]+)"));
    FRegexMatcher Matcher(TokenPattern, Data);

    while (Matcher.FindNext()) {
        Tokens.Add(Matcher.GetCaptureGroup(0));
    }
}

void UITBParser::ParseTokens(const TArray<FString>& Tokens) {
    int32 Index = 0;
    while (Index < Tokens.Num()) {
        if (Tokens[Index] == TEXT("OBJECT")) {
            ParseObject(Tokens, Index);
        }
        else {
            Index++;
        }
    }
}

void UITBParser::ParseObject(const TArray<FString>& Tokens, int32& Index) {
    Index++; // Skip "OBJECT"

    // OBJECT ���� �Ľ�
    while (Index < Tokens.Num()) {
        if (Tokens[Index] == TEXT("POLYGON")) {
            ParsePolygon(Tokens, Index);
        }
        else if (Tokens[Index] == TEXT("]")) {
            Index++; // End of OBJECT
            break;
        }
        else {
            Index++;
        }
    }
}

void UITBParser::ParsePolygon(const TArray<FString>& Tokens, int32& Index) {
    Index++; // Skip "POLYGON"

    FITBPolygon NewPolygon;

    // POLYGON ���� �Ľ�
    while (Index < Tokens.Num()) {
        if (Tokens[Index] == TEXT("[")) {
            Index++; // Skip "["
        }
        else if (Tokens[Index].StartsWith(TEXT("[NORMAL"))) {
            // ��� �� ���ؽ� �Ľ�
            FITBVertex Vertex;

            // ��� �Ľ�
            FString NormalStr = Tokens[Index];
            NormalStr.RemoveFromStart(TEXT("[NORMAL "));
            NormalStr.RemoveFromEnd(TEXT("]"));
            TArray<FString> NormalComponents;
            NormalStr.ParseIntoArray(NormalComponents, TEXT(" "), true);

            if (NormalComponents.Num() >= 3) {
                Vertex.Normal.X = FCString::Atod(*NormalComponents[0]);
                Vertex.Normal.Y = FCString::Atod(*NormalComponents[1]);
                Vertex.Normal.Z = FCString::Atod(*NormalComponents[2]);
            }

            Index++; // Move to Position
            // ��ġ �Ľ�
            if (Index < Tokens.Num()) {
                FString PositionStr = Tokens[Index];
                PositionStr.RemoveFromStart(TEXT("-")); // ���� ó��
                TArray<FString> PositionComponents;
                PositionStr.ParseIntoArray(PositionComponents, TEXT(" "), true);

                if (PositionComponents.Num() >= 3) {
                    Vertex.Position.X = FCString::Atod(*PositionComponents[0]) * -1.0; // ���� ó��
                    Vertex.Position.Y = FCString::Atod(*PositionComponents[1]);
                    Vertex.Position.Z = FCString::Atod(*PositionComponents[2]);
                }
            }

            NewPolygon.Vertices.Add(Vertex);
            Index++;
        }
        else if (Tokens[Index] == TEXT("]")) {
            Index++; // End of POLYGON
            break;
        }
        else {
            Index++;
        }
    }

    Polygons.Add(NewPolygon);
}

#include "ITBFactory.h"
#include "ITBImporter.h"
#include "Engine/StaticMesh.h"
#include "EditorFramework/AssetImportData.h"

UITBFactory::UITBFactory() {
    bEditorImport = true;
    bText = true;
    Formats.Add(TEXT("itb;ITB Model"));
    SupportedClass = UStaticMesh::StaticClass();
}

UObject* UITBFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName,
    EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn,
    bool& bOutOperationCanceled) {
    // ITBImporter�� ����Ͽ� ���� ����Ʈ
    UITBImporter* Importer = NewObject<UITBImporter>();
    UStaticMesh* StaticMesh = Importer->ImportITB(Filename, InName.ToString(), InParent, Flags);

    if (StaticMesh) {
        // ����Ʈ ������ ����
        StaticMesh->AssetImportData->Update(Filename);

        // ����Ʈ ���� �޽���
        Warn->Logf(ELogVerbosity::Display, TEXT("Successfully imported ITB file: %s"), *Filename);

        return StaticMesh;
    }
    else {
        // ����Ʈ ���� �޽���
        Warn->Logf(ELogVerbosity::Error, TEXT("Failed to import ITB file: %s"), *Filename);
        return nullptr;
    }
}

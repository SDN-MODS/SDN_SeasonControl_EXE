// ============================================================================
// PASTA: 4_World/SDN_System
// ARQUIVO: SDN_AnimalSystem.c
// DESCRIÇÃO: Sistema de Controle de Fauna.
// CORREÇÃO: Deleção segura (Delayed) para evitar crash de 'Access Violation' no EEInit.
// ============================================================================

modded class AnimalBase
{
    override void EEInit()
    {
        super.EEInit();

        // Apenas rodamos a lógica no Servidor e se o animal estiver de fato vivo
        if (GetGame().IsServer() && IsAlive())
        {
            SDN_SeasonManager manager = SDN_SeasonManager.GetInstance();

            if (manager && !manager.IsAnimalAllowed(this.GetType()))
            {
                // SÊNIOR FIX: Não excluímos imediatamente (evita crash do EEInit).
                // 1. Ocultamos o animal no limbo (joga pro fundo do mapa) para os jogadores não verem ele aparecer e sumir.
                vector currentPos = this.GetPosition();
                this.SetPosition(Vector(currentPos[0], -1000.0, currentPos[2]));

                // 2. Avisamos a CE (Central Economy) para remover esse objeto na próxima limpeza (3 segundos).
                this.SetLifetime(3.0);

                // 3. Garantimos a exclusão forçada, mas segura, após 3 segundos,
                // usando o wrapper para respeitar a sintaxe de delegate do Enforce Script.
                GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.DelayedDelete, 3000, false);
            }
        }
    }

    // Wrapper para executar o ObjectDelete em CallLater sem quebrar a compilação
    void DelayedDelete()
    {
        GetGame().ObjectDelete(this);
    }
}
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
                // SÊNIOR FIX: Não excluímos usando ObjectDelete ou CallLater pois a Engine
                // DayZ Central Economy gerencia a limpeza do Lifespan por conta própria e pode
                // deletar o objeto no lixo antes do timer.

                // 1. Ocultamos o animal no limbo (joga pro fundo do mapa) para os jogadores não verem ele aparecer e sumir.
                vector currentPos = this.GetPosition();
                this.SetPosition(Vector(currentPos[0], -1000.0, currentPos[2]));

                // 2. Avisamos a CE (Central Economy) para remover esse objeto na próxima limpeza (3 segundos).
                // Mantemos ele VIVO, para não quebrar a cota de restock ativa do servidor (impedindo loop de spawn).
                this.SetLifetime(3.0);

                // 3. Garantimos a exclusão forçada segura usando o wrapper.
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
import QtQuick 2.0
import Sailfish.Silica 1.0
import "../model"
import "../components"
import ru.sashikknox 1.0

Page {
    id: channelsPage
    objectName: "ChannelsPage"
    layer.enabled: true
    property Context context
    property bool isuptodate: false
    property bool first_run: true

    property int server_index
    property int team_index
    property int unread_messages : 0
    property string teamid
    property string team_label

    property int ct_public: MattermostQt.ChannelPublic
    property int ct_private: MattermostQt.ChannelPrivate
    property int ct_direct: MattermostQt.ChannelDirect

    allowedOrientations: Orientation.All

    property ChannelsModel channelsmodel: ChannelsModel {
        mattermost: context.mattermost
    }

    onStatusChanged: {
        if( status == PageStatus.Active && isuptodate == false )
        {
            isuptodate = true;
            context.mattermost.get_public_channels(server_index,teamid)
            context.mattermost.replyFinished.connect( function func(request_type) {
                if( request_type == MattermostQt.rt_get_public_channels ) {
                    first_run = false
                }
            })
        }
    }



    SilicaListView {
        id: channelslist
        width: parent.width
        anchors.fill: parent
        model: channelsmodel
        spacing: Theme.paddingSmall

        PullDownMenu {
            MenuItem {
                text: qsTr("About")
                onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"),{context: channelsPage.context})
            }
            MenuItem {
                text: qsTr("Options")
                onClicked: pageStack.push(Qt.resolvedUrl("OptionsPage.qml"),{context: channelsPage.context})
            }
        }

        VerticalScrollDecorator {}

        header: PageHeader {
            id: pageheader
            title: team_label
        }

        delegate: ListItem {
            id: bgitem

            TouchBlocker {
                anchors.fill: parent
                enabled: m_type != ChannelsModel.Channel
            }

            ParallelAnimation {
                id: panim
                running: first_run
                property int dur: 200
                NumberAnimation  {
                    target: bgitem
                    property: "height"
                    easing.type: Easing.OutQuad
                    from: 0
                    to: channellabel.height;
                    duration: panim.dur
                }
                NumberAnimation {
                    target: bgitem
                    property: "opacity"
                    easing.type: Easing.InExpo
                    from: 0
                    to: 1.0;
                    duration: panim.dur
                }
            }
            width: parent.width

            ChannelLabel {
                id: channellabel
                _display_name: m_display_name
                _purpose: m_purpose
                _header: m_header
                _index: m_index
                _type: m_type
                _channel_unread: role_mention_count + role_msg_unread
                _users_typing: role_users_typing
                channelType: channel_type
                directChannelImage: avatar_path
                directChannelUserStatus: user_status
                context: channelsPage.context

                x: Theme.horizontalPageMargin
                anchors {
                    fill: parent
                    verticalCenter: parent.verticalCenter
                    //left: parent.left
                    //right: parent.right
                    //top: parent.top
                }
                //                    anchors.topMargin: Theme.paddingSmall
                anchors.leftMargin: Theme.paddingLarge
                anchors.rightMargin: Theme.paddingMedium
            }
            onClicked: {
                var messages = pageStack.pushAttached(
                            Qt.resolvedUrl("MessagesPage.qml"),
                            {
                                team_index: channelsPage.team_index,
                                server_index: channelsPage.server_index,
                                channel_index: channellabel._index,
                                channel_type: channellabel.channelType,
                                channel_id: context.mattermost.getChannelId(
                                                channelsPage.server_index,
                                                channelsPage.team_index,
                                                channellabel.channelType,
                                                channellabel._index
                                                ),
                                channel_name: channellabel._display_name,
                                context: channelsPage.context
                            } );
                pageStack.navigateForward(PageStackAction.Animated);
            }
        }
    }
}
